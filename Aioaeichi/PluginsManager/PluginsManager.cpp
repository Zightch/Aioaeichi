#include "PluginsManager.h"
#include "Screw/Logger.h"
#include "DLLLoader/DLLLoader.h"
#include "EventSystem/eventSystem.h"
#include <QDir>

PluginsManager::PluginsManager(QObject *parent) : QObject(parent) {
    if(parent != nullptr) {
        cp = (CommandProcessing *)  parent;
        connect(cp, &CommandProcessing::command, this, &PluginsManager::cmdProc);
        connect(cp, &CommandProcessing::exit, this, &PluginsManager::exit);
        cp->addCmd("loadPlugins", 1);
        cp->addCmd("initPlugins", 1);
        cp->addCmd("startPlugins", 1);
        cp->addCmd("donePlugins", 1);
    }
}

PluginsManager::~PluginsManager() = default;

void PluginsManager::cmdProc(const QByteArray &cmd, const QByteArrayList &args, int cs, bool &isProcess) {
    if (cs == 1) {
        if (cmd == "loadPlugins") {
            isProcess = true;
            loadPlugins();
        } else if (cmd == "initPlugins") {
            isProcess = true;
            initPlugins();
        } else if (cmd == "startPlugins") {
            isProcess = true;
            startPlugins();
        } else if (cmd == "donePlugins") {
            isProcess = true;
            LOG(Info, ("已成功加载" + QString::number(plugins.size()).toLocal8Bit() + "个插件").data());
        }
    }
}

void PluginsManager::exit() {
    if (!plugins.empty()) {
        LOG(Info, "开始卸载插件");
        typedef void(*FP)();
        for (qsizetype i = plugins.size() - 1; i >= 0; i--) {
            FP unload = (FP) getFunction(plugins[i].handle, "unload");
            if (unload != nullptr) {
                LOG(Info, ("正在卸载" + plugins[i].name).data());
                unload();
                LOG(Info, (plugins[i].name + "卸载完成").data());
            } else
                LOG(Info, ("无法正常卸载" + plugins[i].name).data());
        }
        EventSystem::clearListener();
        for(qsizetype i = 0; i < plugins.size(); i++) {
            unloadDLL(plugins[i].handle);
            for (auto lib: plugins[i].libs)
                unloadDLL(lib);
        }
        LOG(Info, "所有插件卸载完成");
    }
}

void PluginsManager::loadPlugins() {
    QDir generalDir;
    if (!generalDir.exists("plugins"))
        generalDir.mkpath("plugins");
    if (!generalDir.exists("plugins-libs"))
        generalDir.mkpath("plugins-libs");
    LOG(Info, "开始加载插件");
    {//遍历所有dll文件
        auto traverseDocument = [&](
                const QByteArray &path,
                const QByteArray &wildcardCharacters = "*.*"
        ) -> QStringList {
            QDir dir(path);
            QStringList nameFilters;
            nameFilters << wildcardCharacters;
            return dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
        };
        QStringList files = traverseDocument("./plugins/", "*.dll");
        for (const QString &file: files) {
            QByteArray filename = file.toLocal8Bit();
            plugins.emplace_back();
            {
                QByteArray libs_path = "plugins-libs/" + filename + "/";
                if (!generalDir.exists(libs_path))
                    generalDir.mkpath(libs_path);
                QStringList libs_files = traverseDocument(libs_path, "*.dll");
                for (const QString &lib: libs_files) {
                    QByteArray libname = lib.toLocal8Bit();
                    void *dllHandle = loadDLL((libs_path + libname).data());
                    if (dllHandle != nullptr)
                        plugins.back().libs.push_back(dllHandle);
                }
            }
            void *dllHandle = loadDLL(("plugins/" + filename).data());
            if (dllHandle != nullptr) {
                plugins.back().handle = dllHandle;
                plugins.back().name = filename;
                LOG(Info, ("已加载" + filename).data());
            } else {
                for (auto lib: plugins.back().libs)
                    unloadDLL(lib);
                plugins.removeLast();
                LOG(Warning, ("无效的插件" + filename).data());
            }
        }
    }//遍历所有dll文件
}

void PluginsManager::initPlugins() {
    if (!plugins.empty()) {
        LOG(Info, "开始初始化插件");
        typedef void(*FP)();
        for (qsizetype i = 0; i < plugins.size(); i++) {
            FP init = (FP) getFunction(plugins[i].handle, "init");
            if (init != nullptr) {
                LOG(Info, ("正在初始化" + plugins[i].name).data());
                init();
                LOG(Info, (plugins[i].name + "初始化完成").data());
            } else {
                LOG(Warning, ("无法初始化" + plugins[i].name).data());
                for (auto lib: plugins[i].libs)
                    unloadDLL(lib);
                unloadDLL(plugins[i].handle);
                plugins.removeAt(i);
                i--;
            }
        }
    }
}

void PluginsManager::startPlugins() {
    if (!plugins.empty()) {
        LOG(Info, "开始启用插件");
        typedef void(*FP)();
        for (qsizetype i = 0; i < plugins.size(); i++) {
            FP start = (FP) getFunction(plugins[i].handle, "start");
            if (start != nullptr) {
                LOG(Info, ("正在启用" + plugins[i].name).data());
                start();
                LOG(Info, (plugins[i].name + "已启用").data());
            } else {
                LOG(Warning, ("无法启用" + plugins[i].name).data());
                for (auto lib: plugins[i].libs)
                    unloadDLL(lib);
                unloadDLL(plugins[i].handle);
                plugins.removeAt(i);
                i--;
            }
        }
    }
}
