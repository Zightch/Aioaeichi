#include "ConsoleCommand.h"
#include "Screw/Logger.h"
#include "EventSystem/eventSystem.h"
#include<iostream>
#ifdef _WIN32
#include <conio.h>
#elif __linux__
#include "terminal.hpp"
#endif

ConsoleCommand *ConsoleCommand_once = nullptr;

ConsoleCommand::ConsoleCommand(QObject *parent) : QThread(parent) {
    cp = (CommandProcessing *) parent;
    Screw::setLogCall((void(*)(...))addLog);
    connect(cp, &CommandProcessing::exit, this, [&]() {
        isRun = false;
        wait();
        exit();
    });
    isRun = true;
    EventSystem::Event addCmdEvent("ConsoleCommand.addCmdEvent");
    auto addCmd = [&](EventSystem::Event *e) {
        auto ce = (*((CommandEvent *) e));
        if (cp != nullptr)
            cp->addCmd(ce);
        e->ignore();
    };
    EventSystem::Listener addCmd_l("ConsoleCommand._cmdProc", addCmd);
    addCmd_l.listenEvent(addCmdEvent);
}

void ConsoleCommand::run() {
    std::cout << '>' << std::flush;
    historyCommand.emplace_back();
    while (isRun) {
        int tmp = 0;
        while ((tmp != '\r') && (tmp != '\n') && isRun) {
            if (kbhit()) {
                tmp = getch();
#ifdef _WIN32
                if (tmp == '\b') {
#elif __linux__
                if (tmp == 127) {
#endif
                    if (tmpIsSave) {
                        currentHistoryCommandIndex = historyCommand.size() - 1;
                        tmpIsSave = false;
                    }
                    if (currentInputLeft.size() != 0) {
                        QByteArray tmp_qba;
                        tmp_qba.append(currentInputLeft, currentInputLeft.size() - 1);
                        currentInputLeft = tmp_qba;
                        std::cout << "\b \b";
                        std::cout << currentInputRight.data() << " \b";
                        for (qsizetype i = 0; i < currentInputRight.size(); i++)
                            std::cout << '\b';
                    }
#ifdef  _WIN32
                } else if (dirReady) {
                    dirReady = false;
                    if (tmp == 72) {
                        currentInputRight = "";
                        if (!tmpIsSave) {
                            tmpIsSave = true;
                            currentInputTmp = currentInput;
                        }
                        if ((historyCommand.size() > 1) && currentHistoryCommandIndex > 0) {
                            for (unsigned long long i = 0; i < currentInputLeft.size(); i++)
                                std::cout << "\b";
                            for (unsigned long long i = 0; i < currentInput.size(); i++)
                                std::cout << " ";
                            for (unsigned long long i = 0; i < currentInput.size(); i++)
                                std::cout << "\b";
                            currentInput = historyCommand[--currentHistoryCommandIndex];
                            currentInputLeft = currentInput;
                            std::cout << currentInput.data();
                        }
                    } else if (tmp == 80) {
                        currentInputRight = "";
                        if (!tmpIsSave) {
                            tmpIsSave = true;
                            currentInputTmp = currentInput;
                        }
                        for (unsigned long long i = 0; i < currentInputLeft.size(); i++)
                            std::cout << "\b";
                        for (unsigned long long i = 0; i < currentInput.size(); i++)
                            std::cout << " ";
                        for (unsigned long long i = 0; i < currentInput.size(); i++)
                            std::cout << "\b";
                        if (currentHistoryCommandIndex < historyCommand.size() - 2) {
                            currentInput = historyCommand[++currentHistoryCommandIndex];
                            currentInputLeft = currentInput;
                            std::cout << currentInput.data();
                        } else {
                            if (currentHistoryCommandIndex != historyCommand.size() - 1)
                                currentHistoryCommandIndex++;
                            tmpIsSave = false;
                            currentInput = currentInputTmp;
                            currentInputLeft = currentInput;
                            std::cout << currentInput.data();
                        }
                    } else if (tmp == 75) {
                        if (currentInputLeft.size() != 0) {
                            char cilb = currentInputLeft[currentInputLeft.size() - 1];
                            QByteArray tmp_qba;
                            tmp_qba.append(currentInputLeft, currentInputLeft.size() - 1);
                            currentInputLeft = tmp_qba;
                            currentInputRight = cilb + currentInputRight;
                            std::cout << '\b';
                        }
                    } else if (tmp == 77) {
                        if (currentInputRight.size() != 0) {
                            char cirb = currentInputRight[0];
                            QByteArray tmp_qba;
                            for (qsizetype i = 1; i < currentInputRight.size(); i++)
                                tmp_qba += currentInputRight[i];
                            currentInputRight = tmp_qba;
                            currentInputLeft += cirb;
                            std::cout << cirb;
                        }
                    }
                } else if (tmp == 224) {
                    dirReady = true;
#elif __linux__
                } else if (terminalControlReady) {
                    terminalControlReady = false;
                    if (tmp == 'A') { // 上
                        currentInputRight = "";
                        if (!tmpIsSave) {
                            tmpIsSave = true;
                            currentInputTmp = currentInput;
                        }
                        if ((historyCommand.size() > 1) && currentHistoryCommandIndex > 0) {
                            for (unsigned long long i = 0; i < currentInputLeft.size(); i++)
                                std::cout << "\b";
                            for (unsigned long long i = 0; i < currentInput.size(); i++)
                                std::cout << " ";
                            for (unsigned long long i = 0; i < currentInput.size(); i++)
                                std::cout << "\b";
                            currentInput = historyCommand[--currentHistoryCommandIndex];
                            currentInputLeft = currentInput;
                            std::cout << currentInput.data();
                        }
                    }
                    else if (tmp == 'B') { // 下
                        currentInputRight = "";
                        if (!tmpIsSave) {
                            tmpIsSave = true;
                            currentInputTmp = currentInput;
                        }
                        for (unsigned long long i = 0; i < currentInputLeft.size(); i++)
                            std::cout << "\b";
                        for (unsigned long long i = 0; i < currentInput.size(); i++)
                            std::cout << " ";
                        for (unsigned long long i = 0; i < currentInput.size(); i++)
                            std::cout << "\b";
                        if (currentHistoryCommandIndex < historyCommand.size() - 2) {
                            currentInput = historyCommand[++currentHistoryCommandIndex];
                            currentInputLeft = currentInput;
                            std::cout << currentInput.data();
                        } else {
                            if (currentHistoryCommandIndex != historyCommand.size() - 1)
                                currentHistoryCommandIndex++;
                            tmpIsSave = false;
                            currentInput = currentInputTmp;
                            currentInputLeft = currentInput;
                            std::cout << currentInput.data();
                        }
                    } else if (tmp == 'D') { // 左
                        if (currentInputLeft.size() != 0) {
                            char cilb = currentInputLeft[currentInputLeft.size() - 1];
                            QByteArray tmp_qba;
                            tmp_qba.append(currentInputLeft, currentInputLeft.size() - 1);
                            currentInputLeft = tmp_qba;
                            currentInputRight = cilb + currentInputRight;
                            std::cout << '\b';
                        }
                    } else if (tmp == 'C') { // 右
                        if (currentInputRight.size() != 0) {
                            char cirb = currentInputRight[0];
                            QByteArray tmp_qba;
                            for (qsizetype i = 1; i < currentInputRight.size(); i++)
                                tmp_qba += currentInputRight[i];
                            currentInputRight = tmp_qba;
                            currentInputLeft += cirb;
                            std::cout << cirb;
                        }
                    }
                } else if (escReady) {
                    escReady = false;
                    if (tmp == 91)
                        terminalControlReady = true;
                    else if (((tmp != '\r') && (tmp != '\n')) && (' ' <= tmp && tmp <= '~'))
                        goto A;
                } else if (tmp == 27) {
                    escReady = true;
#endif
                } else if (((tmp != '\r') && (tmp != '\n')) && (' ' <= tmp && tmp <= '~')) {
A:
                    if (tmpIsSave) {
                        currentHistoryCommandIndex = historyCommand.size() - 1;
                        tmpIsSave = false;
                    }
                    currentInputLeft.push_back((char)tmp);
                    std::cout << (char)tmp;
                    std::cout << currentInputRight.data();
                    for (qsizetype i = 0; i < currentInputRight.size(); i++)
                        std::cout << "\b";
                    std::cout << std::flush;
                }
                if (tmp == '\r' || tmp == '\n')
                    std::cout << std::endl;
                currentInput = currentInputLeft + currentInputRight;
            }
            {
                addLogMutex.lock();
                bool empty = log.empty();
                addLogMutex.unlock();
                if (!empty) {
                    for (unsigned long long i = 0; i <= currentInputLeft.size(); i++)
                        std::cout << "\b";
                    for (unsigned long long i = 0; i <= currentInput.size(); i++)
                        std::cout << " ";
                    for (unsigned long long i = 0; i <= currentInput.size(); i++)
                        std::cout << "\b";
                    addLogMutex.lock();
                    empty = log.empty();
                    addLogMutex.unlock();
                    while (!empty) {
                        addLogMutex.lock();
                        LogInfo li = log.front();
                        log.pop();
                        addLogMutex.unlock();
                        if (li.eInfo) {
                            std::string rank;
                            switch (li.rank) {
                                case LogInfo::Info:
                                    rank = "[I]";
                                    break;
                                case LogInfo::Warning:
                                    rank = "[W]";
                                    break;
                                case LogInfo::Error:
                                    rank = "[E]";
                                    break;
                                default:
                                    break;
                            }
                            std::string file;
                            {
                                auto i = li.file.end();
                                for (; i != li.file.begin(); i--)
                                    if ((*i) == '/' || (*i) == '\\')break;
                                for (i++; i != li.file.end(); i++)
                                    file.push_back(*i);
                            }
                            std::cout << li.time
                                      << rank
                                      << '[' << (void*)li.thread << ']'
                                      << '[' << file << ':'
                                      << li.line << ']' << ' ';
                        }
                        std::cout << li.data << std::endl;
                        addLogMutex.lock();
                        empty = log.empty();
                        addLogMutex.unlock();
                    }
                    std::cout << '>' << currentInput.data();
                    for (qsizetype i = 0; currentInputRight.size(); i++)std::cout << '\b';
                    std::cout << std::flush;
                }
            }
        }
        if ((cp != nullptr) && isRun) {
            cp->addCmd(currentInput.data(), 0);
            if ((currentInput != "") || (!currentInput.isEmpty())) {
                historyCommand.back() = currentInput;
                historyCommand.emplace_back();
            }
            currentHistoryCommandIndex = historyCommand.size() - 1;
            currentInput = "";
            currentInputLeft = "";
            currentInputRight = "";
            std::cout << '>' << std::flush;
        }
    }
}

ConsoleCommand *ConsoleCommand::getObjetc(QObject *parent) {
    if(ConsoleCommand_once == nullptr)ConsoleCommand_once = new ConsoleCommand(parent);
    return ConsoleCommand_once;
}

void ConsoleCommand::deleteThis() {
    delete ConsoleCommand_once;
    ConsoleCommand_once = nullptr;
}

[[maybe_unused]] void ConsoleCommand::exit() {
    EventSystem::deleteEvent("ConsoleCommand.addCmdEvent");
}

void ConsoleCommand::addLog(
            bool eInfo,//是否有时间等信息
            const std::string &time,//时间
            LogInfo::LogRank rank,//日志等级
            unsigned long long thread,//当前线程
            const std::string &file,//所在文件
            unsigned long long line,//行号
            const std::string &data//内容
) {
    if (ConsoleCommand_once != nullptr) {
        ConsoleCommand_once->addLogMutex.lock();
        ConsoleCommand_once->log.push({eInfo, time, rank, thread, file, line, data});
        ConsoleCommand_once->addLogMutex.unlock();
    }
}
