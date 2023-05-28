#pragma once

#ifdef _WIN32
#ifdef SCREW_EXPORT
#define SCREW __declspec(dllexport)
#else
#define SCREW __declspec(dllimport)
#endif
#elif __linux__
#define SCREW
#endif

#include<string>
namespace Screw {
    enum LogRank {
        Info,
        Warning,
        Error,
    };

    SCREW void setLogCall(void(*)(...));

    SCREW void logger(
            LogRank logRank,
            const std::string &file,
            unsigned long long line,
            const std::string &msg
    );

    SCREW void logger(const std::string &msg);
}

#define LOG(logRank, msg) Screw::logger(Screw::LogRank::logRank, __FILE__, __LINE__, msg)
#define LOG_NO_INFO(msg) Screw::logger(msg)
