#pragma once

#ifdef _WIN32
#define SCREW __declspec(dllimport)
#elif __linux__
#define SCREW
#endif

#include<QByteArray>

namespace Logger {
    enum LogRank {
        Info,
        Warning,
        Error,
    };

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
