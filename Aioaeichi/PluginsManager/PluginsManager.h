#pragma once
#include "CommandProcessing/CommandProcessing.h"

class PluginsManager : public QObject {
Q_OBJECT
public:

    explicit PluginsManager(QObject * = nullptr);
    ~PluginsManager() override;

private:

    void loadPlugins();

    void initPlugins();

    void startPlugins();

    void cmdProc(const QByteArray &, const QByteArrayList &, int, bool &);

    void exit();

    class Plugin {
    public:
        void *handle = nullptr;
        QList<void *> libs;
        QByteArray name;
    };

    QList<Plugin> plugins;
    CommandProcessing *cp = nullptr;
};
