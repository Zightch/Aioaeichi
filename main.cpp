#include<QGuiApplication>
#include"Aioaeichi/ConsoleCommand/ConsoleCommand.h"
#include"Aioaeichi/CommandProcessing/CommandProcessing.h"
#include"Aioaeichi/PluginsManager/PluginsManager.h"
#include<iostream>
int main(int argc, char *argv[]) {
    QGuiApplication a(argc, argv);
    CommandProcessing cp;
    PluginsManager pm(&cp);
    auto cc = ConsoleCommand::getObjetc(&cp);
    QObject::connect(&cp, &CommandProcessing::exit, &a, &QGuiApplication::quit);
    cc->start();
    int exec = QGuiApplication::exec();
    ConsoleCommand::deleteThis();
    std::cout << std::endl;
    return exec;
}
