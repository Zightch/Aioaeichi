#include "CmdIss.h"
#include "Logger/Logger.h"
#include <QThread>
#include <utility>
#include <random>
#include <QDateTime>

CmdIss *CmdIss::once = nullptr;

void CmdIss::addCmd_(const QByteArray &src) {
    QMutexLocker ml(&mutex);
    if (!src.isEmpty()) {
        QByteArray cmd;
        QList<QByteArray> args;
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
        cmdBuf.push_back({cmd, args});
    }
}

CmdIss::CmdIss(QThread *thread) {
    moveToThread(thread);
    QMutexLocker ml(&mutex);
    connect(this, &CmdIss::timerStartS_, this, &CmdIss::timerStart_, Qt::QueuedConnection);
    connect(this, &CmdIss::startExitInAdvanceS_, this, &CmdIss::startExit_, Qt::QueuedConnection);
    connect(this, &CmdIss::appExitInAdvanceS_, this, &CmdIss::appExit_, Qt::QueuedConnection);
    emit timerStartS_();
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

void CmdIss::cmdProc_() {
    QMutexLocker ml(&mutex);
    if (!isReadyExit) {
        while (!cmdBuf.isEmpty()) {
            auto cmdTmp = cmdBuf.front();
            cmdBuf.pop_front();
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
                connect(this, &CmdIss::startExit, this, &CmdIss::defaultCanExit_, Qt::QueuedConnection);
                emit startExitInAdvanceS_();
            }
            if ((!isProc))
                LOG(Info, "\"" + cmd + "\"未知的命令");
            isProc = false;
            isAccept = false;
        }
    } else if (isCanExit)
            emit appExitInAdvanceS_();
}

CmdIss::~CmdIss() = default;

void CmdIss::addCmdCallBack(CmdProc* ccb) {
    QMutexLocker ml(&mutex);
    if (isIss) {
        LOG(Warning, "警告: CmdIss正在分发命令, 此过程不允许添加新的Cmd处理器");
        return;
    }
    if (ccb != nullptr)
        cmdCallBack.push_back(ccb);
}

void CmdIss::rmCmdCallBack(CmdProc* ccb) {
    QMutexLocker ml(&mutex);
    if (isIss) {
        LOG(Warning, "警告: CmdIss正在分发命令, 此过程不允许删除Cmd处理器");
        return;
    }
    if (ccb != nullptr)
        cmdCallBack.removeOne(ccb);
}

void CmdIss::canExit(unsigned long long ID) {
    QMutexLocker ml(&mutex);
    if (isReadyExit) {
        if (ID != 0)
            if (activ.contains(ID))
                activ[ID] = true;
        isCanExit = true;
        for (auto i: activ)
            if (!i) isCanExit = false;
    }
}

void CmdIss::startExit_() {
    QMutexLocker ml(&mutex);
    emit startExit();
}

void CmdIss::appExit_() {
    QMutexLocker ml(&mutex);
    delete timer;
    timer = nullptr;
    emit appExit();
}

unsigned long long CmdIss::registActiv() {
    QMutexLocker ml(&mutex);
    unsigned long long tmp;
    std::mt19937_64 gen(QDateTime::currentMSecsSinceEpoch());
    std::uniform_int_distribution<unsigned long long> dis;
    do { tmp = dis(gen); } while (activ.contains(tmp));
    activ[tmp] = false;
    return tmp;
}

void CmdIss::rmActiv(unsigned long long ID) {
    QMutexLocker ml(&mutex);
    if (ID != 0)
        if (activ.contains(ID))
            activ.remove(ID);
}

void CmdIss::defaultCanExit_() {
    QMutexLocker ml(&mutex);
    canExit();
}

void CmdIss::timerStart_() {
    QMutexLocker ml(&mutex);
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&CmdIss::cmdProc_);
    timer->start(100);
}

CmdIss::CmdProc::CmdProc(CmdIss::CmdCallBack ccb) : fn(std::move(ccb)) {}
