#include "PluginsManager.h"
#include "Screw/Logger.h"
#include "Screw/CmdIss.h"
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
        for (auto &i: plugins) {
            FP unload = i.plugin->resolve("unload");
            if (unload != nullptr) {
                LOG(Info, ("正在卸载" + i.name).toUtf8());
                unload();
                LOG(Info, (i.name + "卸载完成").toUtf8());
            } else
                LOG(Error, ("无法正常卸载" + i.name).toUtf8());
        }
        freeLibs_();
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
        auto traverseDocument = [](const QString &path, const QString &wildcardCharacters = "*.*") -> QStringList {
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
            plugins.emplace_back();
            {
                QString libsPath = "plugins-libs/" + file + "/";
                if (!generalDir.exists(libsPath))
                    generalDir.mkpath(libsPath);
#ifdef _WIN32
                QStringList libFiles = traverseDocument(libsPath, "*.dll");
#elif __linux__
                QStringList libFiles = traverseDocument(libsPath, "*.so");
#endif
                for (const QString &libFile: libFiles) {
                    auto lib = new QLibrary(libsPath + libFile);
                    if (lib->load())
                        plugins.back().libs.push_back(lib);
                }
            }
            auto plugin = new QLibrary("plugins/" + file);
            if (plugin->load()) {
                plugins.back().plugin = plugin;
                plugins.back().name = file;
                LOG(Info, ("已加载" + file).toUtf8());
            } else {
                for (auto lib: plugins.back().libs) {
                    lib->unload();
                    delete lib;
                }
                delete plugin;
                plugins.removeLast();
                LOG(Warning, ("无效的插件" + file).toUtf8());
            }
        }
    }//遍历所有dll文件
    emit initPluginsS_();
}

void PluginsManager::initPlugins_() {
    if (!plugins.empty()) {
        LOG(Info, "开始初始化插件");
        typedef void(*FP)();
        for (int i = 0; i < plugins.size(); i++) {
            FP init = plugins[i].plugin->resolve("init");
            if (init != nullptr) {
                LOG(Info, ("正在初始化" + plugins[i].name).toUtf8());
                init();
                LOG(Info, (plugins[i].name + "初始化完成").toUtf8());
            } else {
                LOG(Warning, ("无法初始化" + plugins[i].name).toUtf8());
                plugins[i].plugin->unload();
                delete plugins[i].plugin;
                for (auto lib: plugins[i].libs) {
                    lib->unload();
                    delete lib;
                }
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
        for (int i = 0; i < plugins.size(); i++) {
            FP start = plugins[i].plugin->resolve("start");
            if (start != nullptr) {
                LOG(Info, ("正在启用" + plugins[i].name).toUtf8());
                start();
                LOG(Info, (plugins[i].name + "启用完成").toUtf8());
            } else {
                LOG(Warning, ("无法启用" + plugins[i].name).toUtf8());
                plugins[i].plugin->unload();
                delete plugins[i].plugin;
                for (auto lib: plugins[i].libs) {
                    lib->unload();
                    delete lib;
                }
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

void PluginsManager::freeLibs_() {
    for (auto &i: plugins) {
        i.plugin->unload();
        delete i.plugin;
        for (auto lib: i.libs) {
            lib->unload();
            delete lib;
        }
    }
    plugins.clear();
}
