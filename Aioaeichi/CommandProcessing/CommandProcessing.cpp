#include "CommandProcessing.h"
#include "Screw/Logger.h"
#include "EventSystem/eventSystem.h"
CommandProcessing::CommandProcessing(QObject *parent) : QObject(parent) {
    EventSystem::Event("CommandProcessing.commandEvent");
    connect(&timer, &QTimer::timeout, this, &CommandProcessing::cmdProc);
    timer.start(100);
}

void CommandProcessing::addCmd(const QByteArray &cmd_e, int cs) {
    if((cmd_e != "") && (!cmd_e.isEmpty())){
        QByteArray cmd;
        QByteArrayList args;
        bool isQuoting = false;
        for (unsigned long long i = 0; i < cmd_e.size(); i++) {
            if (cmd_e[(qsizetype) i] == ' ' && (!isQuoting)) {
                if (i > 0) {
                    if (cmd_e[(qsizetype) i - 1] != ' ') {
                        args.push_back(QByteArray());
                    }
                }
            } else if (cmd_e[(qsizetype) i] == '\"') {
                if (isQuoting)isQuoting = false;
                else isQuoting = true;
            } else {
                if (args.empty()) {
                    cmd.push_back(cmd_e[(qsizetype) i]);
                } else {
                    args.back().push_back(cmd_e[(qsizetype) i]);
                }
            }
        }
        ceBuff.emplace("CommandProcessing.commandEvent", cmd, args, &isProcess, cs);
    }
}

void CommandProcessing::addCmd(const CommandEvent &ce_e) {
    CommandEvent ce("CommandProcessing.commandEvent");
    ce.setCmd(ce_e.getCmd());
    ce.setArgs(ce_e.getArgs());
    ce.setCommandSender(ce_e.getCommandSender());
    ce.setProcPtr(&isProcess);
    ceBuff.push(ce);
}

void CommandProcessing::cmdProc() {
    while (!ceBuff.empty()) {
        CommandEvent ceTmp = ceBuff.front();
        ceBuff.pop();
        emit command(ceTmp.getCmd(), ceTmp.getArgs(), ceTmp.getCommandSender(), isProcess);
        if ((!isProcess))
            EventSystem::trigger(ceTmp);
        QByteArray cmd = ceTmp.getCmd();
        if ((cmd == "exit") && (!isProcess)) {
            timer.stop();
            EventSystem::deleteEvent("CommandProcessing.commandEvent");
            emit exit();
            return;
        }
        if ((!isProcess))
            LOG(Info, ("\"" + cmd + "\"未知的命令").data());
        isProcess = false;
    }
}
