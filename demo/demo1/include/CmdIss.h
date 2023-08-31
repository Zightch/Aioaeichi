#pragma once
#include <functional>
#include <QTimer>
#include <QMap>
#include <QRecursiveMutex>

#ifdef _WIN32
#define AIOAEICHI __declspec(dllimport)
#elif __linux__
#define AIOAEICHI
#endif

class AIOAEICHI CmdIss : public QObject {//命令下发器
Q_OBJECT

public:

    static CmdIss *getObject(QThread * = nullptr);

    static void deleteObject();

    using CmdCallBack = std::function<void(const QByteArray &, const QByteArrayList &, bool &, bool &)>;

    class AIOAEICHI CmdProc {
    public:
        explicit CmdProc(CmdCallBack ccb);

        CmdProc(const CmdProc &) = delete;

        CmdProc &operator=(const CmdProc &) = delete;

    private:
        CmdCallBack fn = nullptr;

        friend class CmdIss;
    };

    void addCmd(const QByteArray &);

    void addCmdCallBack(CmdProc *);

    void rmCmdCallBack(CmdProc *);

    void canExit(void * = nullptr);

    void registExitActiv(void *);

    void rmExitActiv(void *);

public:
signals:

    void startExit();

    void appExit();

private:
signals:

    void timerStartS_();

    void startExitInAdvanceS_();

    void appExitInAdvanceS_();

private:
    explicit CmdIss(QThread * = nullptr);

    ~CmdIss() override;

    class Cmd {
    public:
        QByteArray cmd;
        QList<QByteArray> args;
    };

    void timerStart_();

    void cmdProc_();

    void startExit_();

    void appExit_();

    void defaultCanExit_();

    QList<CmdProc *> cmdCallBack;
    bool isProc = false;
    bool isAccept = false;
    bool isIss = false;
    bool isReadyExit = false;
    bool isCanExit = false;
    QTimer *timer = nullptr;
    QList<Cmd> cmdBuf;
    QRecursiveMutex mutex;
    QMap<void *, bool> exitActic;
    static CmdIss *once;
};
