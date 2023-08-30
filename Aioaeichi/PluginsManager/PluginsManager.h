#pragma once
#include <QObject>

class PluginsManager : public QObject {
Q_OBJECT
public:

    static PluginsManager *getObject(QThread * = nullptr);

    static void deleteObject();

private:
    explicit PluginsManager(QThread * = nullptr);

    ~PluginsManager() override;

    void loadPlugins();

    void initPlugins();

    void startPlugins();

    void exit();

    class Plugin {
    public:
        void *handle = nullptr;
        QList<void *> libs;
        QByteArray name;
    };

    QList<Plugin> plugins;
    static PluginsManager *once;
};
