#pragma once
#include<QThread>
#include<QMutex>
#include"../CommandProcessing/CommandProcessing.h"

class ConsoleCommand: public QThread {
public:
Q_OBJECT

public:

    static ConsoleCommand *getObjetc(QObject * = nullptr);

    static void deleteThis();

private:
    class LogInfo {
    public:
        enum LogRank {
            Info,
            Warning,
            Error,
        };
        bool eInfo;
        std::string time;//时间
        LogRank rank;//日志等级
        unsigned long long thread;//当前线程
        std::string file;//所在文件
        unsigned long long line;//行号
        std::string data;//内容
    };

    explicit ConsoleCommand(QObject * = nullptr);

    void run() override;

    static void exit();

    static void addLog(
            bool,//是否有时间等信息
            const std::string &,//时间
            LogInfo::LogRank,//日志等级
            unsigned long long,//当前线程
            const std::string &,//所在文件
            unsigned long long,//行号
            const std::string &//内容
    );

    CommandProcessing *cp = nullptr;
    volatile bool isRun = false;
    //volatile bool isRunning = false;
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
    QMutex addLogMutex;
    std::queue<LogInfo> log;
};
