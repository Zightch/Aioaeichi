#pragma once

#include "EventSystem/Event.h"
#include <QByteArray>
#include <QList>

#ifdef _WIN32
#ifdef SCREW_EXPORT
#define SCREW __declspec(dllexport)
#else
#define SCREW __declspec(dllimport)
#endif
#elif __linux__
#define SCREW
#endif

class SCREW CommandEvent : public EventSystem::Event {
public:

    explicit CommandEvent(const char *, int= 0);

    explicit CommandEvent(const char *, QByteArray, bool *, int= 0);

    explicit CommandEvent(const char *, QByteArray, QByteArrayList, bool *, int= 0);

    [[maybe_unused]]
    void setCmd(const QByteArray &);

    [[maybe_unused]]
    void setArgs(const QByteArrayList &);

    [[maybe_unused]]
    void setCommandSender(int);

    [[maybe_unused]]
    void setProcPtr(bool *);

    [[maybe_unused]] [[nodiscard]]
    QByteArray getCmd() const;

    [[maybe_unused]] [[nodiscard]]
    QByteArrayList getArgs() const;

    [[maybe_unused]] [[nodiscard]]
    int getCommandSender() const;

    [[maybe_unused]]
    void proc();

    [[maybe_unused]]
    void noProc();

    [[maybe_unused]] [[nodiscard]]
    bool isProc() const;

protected:
    QByteArray cmd;
    QByteArrayList args;
    bool *_proc = nullptr;
    int commandSender;
};
