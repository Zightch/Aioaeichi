#include "PluginsManager.h"
#include "Logger/Logger.h"
#include "DLLLoader/DLLLoader.h"
#include "CmdIss/CmdIss.h"
#include <QDir>
#include <QThread>

PluginsManager *PluginsManager::once = nullptr;

PluginsManager::PluginsManager(QThread *thread) {
    moveToThread(thread);
    connect(CmdIss::getObject(),&CmdIss::appExit,this,&PluginsManager::exit_);
    connect(this,&PluginsManager::loadPluginsS_,this,&PluginsManager::loadPlugins_,Qt::QueuedConnection);
    connect(this,&PluginsManager::initPluginsS_,this,&PluginsManager::initPlugins_,Qt::QueuedConnection);
    connect(this,&PluginsManager::startPluginsS_,this,&PluginsManager::startPlugins_,Qt::QueuedConnection);
    connect(this,&PluginsManager::doneLoadPluginsS_,this,&PluginsManager::doneLoadPlugins_,Qt::QueuedConnection);
    emit loadPluginsS_();
}

PluginsManager::~PluginsManager() = default;

void PluginsManager::exit_() {
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
        for(auto & plugin : plugins) {
            unloadDLL(plugin.handle);
            for (auto lib: plugin.libs)
                unloadDLL(lib);
        }
        LOG(Info, "所有插件卸载完成");
    }
}

void PluginsManager::loadPlugins_() {
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
#ifdef _WIN32
        QStringList files = traverseDocument("./plugins/", "*.dll");
#elif __linux__
        QStringList files = traverseDocument("./plugins/", "*.so");
#endif
        for (const QString &file: files) {
            QByteArray filename = file.toLocal8Bit();
            plugins.emplace_back();
            {
                QByteArray libs_path = "plugins-libs/" + filename + "/";
                if (!generalDir.exists(libs_path))
                    generalDir.mkpath(libs_path);
#ifdef _WIN32
                QStringList libs_files = traverseDocument(libs_path, "*.dll");
#elif __linux__
                QStringList libs_files = traverseDocument(libs_path, "*.so");
#endif
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
    emit initPluginsS_();
}

void PluginsManager::initPlugins_() {
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
    emit startPluginsS_();
}

void PluginsManager::startPlugins_() {
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
    emit doneLoadPluginsS_();
}

void PluginsManager::deleteObject() {
    delete once;
    once = nullptr;
}

PluginsManager *PluginsManager::getObject(QThread *thread) {
    if (once == nullptr && thread != nullptr)
        once = new PluginsManager(thread);
    return once;
}

void PluginsManager::doneLoadPlugins_() {
    LOG(Info, ("已成功加载" + QString::number(plugins.size()).toLocal8Bit() + "个插件").data());
}
