#pragma once

#ifdef _WIN32
#define SCREW __declspec(dllimport)
#elif __linux__
#define SCREW
#endif

#include<QByteArray>
#include<functional>

namespace Logger {
    enum LogRank {
        Info,
        Warning,
        Error,
    };

    typedef std::function<void(
            bool,//是否有时间等信息
            const QByteArray &,//时间
            LogRank,//日志等级
            unsigned long long,//当前线程
            const QByteArray &,//所在文件
            unsigned long long,//行号
            const QByteArray &//内容
    )> LogCallBack;

    SCREW void setLogCall(const LogCallBack &callBack);

    SCREW void logger(
            LogRank logRank,
            const QByteArray &file,
            unsigned long long line,
            const QByteArray &msg
    );

    SCREW void logger(const QByteArray &msg);
}

#define LOG(logRank, msg) Logger::logger(Logger::LogRank::logRank, __FILE__, __LINE__, msg)
#define LOG_NO_INFO(msg) Logger::logger(msg)
