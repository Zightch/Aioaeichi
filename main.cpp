#include<QCoreApplication>
#include"Screw/CmdIss.h"
#include"Shell/Shell.h"
#include"PluginsManager/PluginsManager.h"
#include<iostream>
#include<QThread>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    QThread thread(&a);
    Shell::getObject(&a);
    CmdIss::getObject(&thread);
    PluginsManager::getObject(&thread);
    QObject::connect(CmdIss::getObject(), &CmdIss::appExit, [&]() {
        thread.exit();
        QCoreApplication::exit();
    });
    thread.start();
    int ret = QCoreApplication::exec();
    thread.wait();
    CmdIss::deleteObject();
    Shell::deleteObject();
    PluginsManager::getObject()->freeLibs_();
    PluginsManager::deleteObject();
    std::cout << std::endl;
    return ret;
}
