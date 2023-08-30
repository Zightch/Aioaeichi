#include "CmdIss.h"
#include "Logger/Logger.h"
#include <QThread>

CmdIss *CmdIss::once = nullptr;

void CmdIss::addCmd(const QByteArray &src) {
    if (!src.isEmpty()) {
        QByteArray cmd;
        std::vector<QByteArray> args;
        bool isQuoting = false;
        for (qsizetype i = 0; i < src.size(); i++) {
            if (src[i] == ' ' && (!isQuoting)) {
                if (i > 0)
                    if (src[i - 1] != ' ')
                        args.emplace_back("");
            } else if (src[i] == '\"') {
                if (isQuoting)isQuoting = false;
                else isQuoting = true;
            } else {
                if (args.empty())
                    cmd.push_back(src[i]);
                else
                    args.back().push_back(src[i]);
            }
        }
        mutex.lock();
        cmdBuf.push_back({cmd, args});
        mutex.unlock();
    }
}

CmdIss::CmdIss(QThread *thread) {
    moveToThread(thread);
    connect(this, &CmdIss::startExitInAdvance, this, &CmdIss::startExit_, Qt::QueuedConnection);
    connect(this, &CmdIss::appExitInAdvance, this, &CmdIss::appExit_, Qt::QueuedConnection);
    connect(this, &CmdIss::timerStart, this, &CmdIss::timerStart_, Qt::QueuedConnection);
    emit timerStart();
}

void CmdIss::deleteObject() {
    delete once;
    once = nullptr;
}

CmdIss *CmdIss::getObject(QThread *thread) {
    if (once == nullptr && thread != nullptr)
        once = new CmdIss(thread);
    return once;
}

void CmdIss::cmdProc() {
    if (!isReadyExit) {
        mutex.lock();
        auto isEmpty = cmdBuf.empty();
        mutex.unlock();
        while (!isEmpty) {
            mutex.lock();
            auto cmdTmp = cmdBuf.front();
            cmdBuf.pop_front();
            mutex.unlock();
            isIss = true;
            for (auto i: cmdCallBack) {
                isAccept = true;
                i->fn(cmdTmp.cmd, cmdTmp.args, isProc, isAccept);
                if (isAccept)break;
            }
            isIss = false;
            auto cmd = cmdTmp.cmd;
            if ((cmd == "exit") && (!isProc) && (!isAccept)) {
                isReadyExit = true;
                isProc = true;
                isAccept = false;
                emit startExitInAdvance();
            }
            if ((!isProc))
                LOG(Info, "\"" + cmd + "\"未知的命令");
            isProc = false;
            isAccept = false;
            mutex.lock();
            isEmpty = cmdBuf.empty();
            mutex.unlock();
        }
    } else {
        mutex.lock();
        if (isCanExit)
            emit appExitInAdvance();
        mutex.unlock();
    }
}

CmdIss::~CmdIss() = default;

void CmdIss::addCmdCallBack(CmdProc* ccb) {
    if (isIss) {
        LOG(Warning, "警告: CmdIss正在分发命令, 此过程不允许添加新的Cmd处理器");
        return;
    }
    if (ccb != nullptr)
        cmdCallBack.push_back(ccb);
}

void CmdIss::rmCmdCallBack(CmdProc* ccb) {
    if (isIss) {
        LOG(Warning, "警告: CmdIss正在分发命令, 此过程不允许删除Cmd处理器");
        return;
    }
    if (ccb != nullptr) {
        auto i = std::find(cmdCallBack.begin(), cmdCallBack.end(), ccb);
        if (i != cmdCallBack.end())
            cmdCallBack.erase(i);
    }
}

void CmdIss::canExit(QObject *obj) {
    mutex.lock();
    if (isReadyExit) {
        exitActic[obj] = true;
        isCanExit = true;
        for (auto i: exitActic)
            if (!i) isCanExit = false;
    }
    mutex.unlock();
}

void CmdIss::startExit_() {
    emit startExit();
}

void CmdIss::appExit_() {
    delete timer;
    timer = nullptr;
    emit appExit();
}

void CmdIss::registExitActiv(QObject *obj) {
    if (!exitActic.contains(obj))
        exitActic.insert(obj, false);
}

void CmdIss::rmExitActiv(QObject *obj) {
    if (exitActic.contains(obj))
        exitActic.remove(obj);
}

void CmdIss::timerStart_() {
    timer = new QTimer(this);
    timer->start(100);
    connect(timer, &QTimer::timeout, this, &CmdIss::cmdProc);
}
