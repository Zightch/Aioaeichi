#include "Logger.h"
#include <QMutexLocker>
#include <thread>

namespace Logger {

    LogCallBack callBack = nullptr;

    void setLogCall(const LogCallBack &lc) {
        if (callBack == nullptr)
            callBack = lc;
    }

    QMutex sctv;
    QMutex vllsis;
    QMutex vls;

    unsigned long long currThread() {
        std::thread::id id = std::this_thread::get_id();
        return *(unsigned long long *) &id;
    }

    QByteArray currTime() {
        QMutexLocker ml(&sctv);
        char tmp[23] = {0};
        time_t ptime;
        time(&ptime);
        strftime(tmp, 22, "[%Y-%m-%d %H:%M:%S]", localtime(&ptime));
        return tmp;
    }

    void logger(
            LogRank logRank,
            const QByteArray &file,
            unsigned long long line,
            const QByteArray &msg
    ) {
        QMutexLocker ml(&vllsis);
        if (callBack != nullptr)
            callBack(true, currTime(), logRank, currThread(), file, line, msg);
    }

    void logger(const QByteArray &msg) {
        QMutexLocker ml(&vls);
        if (callBack != nullptr)
            callBack(false, "", Info, 0, "", 0, msg);
    }
}
