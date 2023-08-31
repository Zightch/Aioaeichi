#pragma once
#include <QObject>

class PluginsManager : public QObject {
Q_OBJECT

public:

    static PluginsManager *getObject(QThread * = nullptr);

    static void deleteObject();

private:
signals:

    void loadPluginsS_();

    void initPluginsS_();

    void startPluginsS_();

    void doneLoadPluginsS_();

private:
    explicit PluginsManager(QThread * = nullptr);

    ~PluginsManager() override;

    void loadPlugins_();

    void initPlugins_();

    void startPlugins_();

    void doneLoadPlugins_();

    void exit_();

    class Plugin {
    public:
        void *handle = nullptr;
        QList<void *> libs;
        QByteArray name;
    };

    QList<Plugin> plugins;
    static PluginsManager *once;
};
