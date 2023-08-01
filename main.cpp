#include<QCoreApplication>
#include"Aioaeichi/ConsoleCommand/ConsoleCommand.h"
#include"Aioaeichi/CommandProcessing/CommandProcessing.h"
#include"Aioaeichi/PluginsManager/PluginsManager.h"
#include<iostream>
int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    CommandProcessing cp;
    PluginsManager pm(&cp);
    auto cc = ConsoleCommand::getObjetc(&cp);
    QObject::connect(&cp, &CommandProcessing::exit, &a, &QCoreApplication::quit);
    cc->start();
    int exec = QCoreApplication::exec();
    ConsoleCommand::deleteThis();
    std::cout << std::endl;
    return exec;
}
