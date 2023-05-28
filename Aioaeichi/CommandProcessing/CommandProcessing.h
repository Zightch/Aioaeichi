#pragma once
#include<QByteArray>
#include<queue>
#include<QTimer>
#include"Screw/CommandEvent.h"
class CommandProcessing : public QObject {
public:
Q_OBJECT
public:
    explicit CommandProcessing(QObject * = nullptr);

    void addCmd(const QByteArray &, int);

    void addCmd(const CommandEvent&);

public:
signals:

    void command(const QByteArray &, const QByteArrayList &, int, bool &);

    void exit();

private:
    void cmdProc();
    bool isProcess = false;
    QTimer timer;
    std::queue<CommandEvent> ceBuff;
};
