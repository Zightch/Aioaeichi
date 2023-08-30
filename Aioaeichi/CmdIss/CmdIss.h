#pragma once
#include <QObject>
#include <functional>
#include <utility>
#include <QTimer>
#include <QMutexLocker>
#include <QMap>

#ifdef _WIN32
#define AIOAEICHI __declspec(dllexport)
#elif __linux__
#define AIOAEICHI
#endif

class AIOAEICHI CmdIss : public QObject {//命令下发器
Q_OBJECT

public:

    static CmdIss *getObject(QThread * = nullptr);

    static void deleteObject();

    using CmdCallBack = std::function<void(const QByteArray &, const std::vector<QByteArray> &, bool &, bool &)>;

    class AIOAEICHI CmdProc {
    public:
        explicit CmdProc(CmdCallBack ccb) : fn(std::move(ccb)) {};

        CmdProc(const CmdProc &) = delete;

        CmdProc &operator=(const CmdProc &) = delete;

    private:
        CmdCallBack fn = nullptr;

        friend class CmdIss;
    };

    void addCmd(const QByteArray &);

    void addCmdCallBack(CmdProc *);

    void rmCmdCallBack(CmdProc *);

    void canExit(QObject *);

    void registExitActiv(QObject *);

    void rmExitActiv(QObject *);

public:
signals:

    void startExit();

    void appExit();

private:
signals:

    void timerStart();

    void startExitInAdvance();

    void appExitInAdvance();

private:
    explicit CmdIss(QThread * = nullptr);

    ~CmdIss() override;

    class Cmd {
    public:
        QByteArray cmd;
        std::vector<QByteArray> args;
    };

    void timerStart_();

    void cmdProc();

    void startExit_();

    void appExit_();

    std::vector<CmdProc *> cmdCallBack;
    bool isProc = false;
    bool isAccept = false;
    bool isIss = false;
    bool isReadyExit = false;
    bool isCanExit = false;
    QTimer *timer = nullptr;
    QList<Cmd> cmdBuf;
    QMutex mutex;
    QMap<QObject *, bool> exitActic;
    static CmdIss *once;
};
