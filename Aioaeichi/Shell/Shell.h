#pragma once

#include <QRecursiveMutex>
#include <QObject>
#include <QList>
#include "Screw/Logger.h"

class Shell : public QObject {
Q_OBJECT

public:

    static Shell *getObject(QObject * = nullptr);

    static void deleteObject();

private:
signals:

    void loopS_();
    void endLoopS_();

private:

    explicit Shell(QObject * = nullptr);

    ~Shell() override;

    void addLog_(
            bool,//是否有时间等信息
            const QByteArray &,//时间
            Logger::LogRank,//日志等级
            unsigned long long,//当前线程
            const QByteArray &,//所在文件
            unsigned long long,//行号
            const QByteArray &//内容
    );

    class LogInfo {
    public:
        bool eInfo;
        QByteArray time;//时间
        Logger::LogRank rank;//日志等级
        unsigned long long thread;//当前线程
        QByteArray file;//所在文件
        unsigned long long line;//行号
        QByteArray data;//内容
    };

    void loop_();
    void endLoop_();

    void up_();
    void down_();
    void left_();
    void right_();

    QByteArray currentInput;
    QByteArray currentInputLeft;
    QByteArray currentInputRight;
    QByteArray currentInputTmp;
    QByteArrayList historyCommand;
#ifdef _WIN32
    bool dirReady = false;
#elif __linux__
    bool escReady = false;
    bool terminalControlReady = false;
#endif
    bool tmpIsSave = false;
    qsizetype currentHistoryCommandIndex = 0;
    QRecursiveMutex addLogMutex;
    QList<LogInfo> log;
    static Shell *once;
};
