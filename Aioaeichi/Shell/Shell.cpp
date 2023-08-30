#include "Shell.h"
#include <iostream>
#include "CmdIss/CmdIss.h"
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#elif __linux__
#include "tools/terminal.hpp"
#endif

Shell* Shell::once = nullptr;

Shell *Shell::getObject(QObject *parent) {
    if (once == nullptr)
        once = new Shell(parent);
    return once;
}

void Shell::deleteObject() {
    delete once;
    once = nullptr;
}

void Shell::addLog_(
        bool eInfo,//是否有时间等信息
        const QByteArray &time,//时间
        Logger::LogRank rank,//日志等级
        unsigned long long thread,//当前线程
        const QByteArray &file,//所在文件
        unsigned long long line,//行号
        const QByteArray &data//内容
) {
    addLogMutex.lock();
    log.push_back({eInfo, time, rank, thread, file, line, data});
    addLogMutex.unlock();
}

Shell::Shell(QObject*parent) : QObject(parent) {
    using namespace std::placeholders;
    Logger::setLogCall(std::bind(&Shell::addLog_, this, _1, _2, _3, _4, _5, _6, _7));
    connect(this, &Shell::loop, this, &Shell::loop_, Qt::QueuedConnection);
    connect(this, &Shell::endLoop, this, &Shell::endLoop_, Qt::QueuedConnection);
    std::cout << '>' << std::flush;
    historyCommand.emplace_back();
    emit loop();
}

Shell::~Shell() = default;

void Shell::endLoop_() {
    if (CmdIss::getObject() != nullptr)
        CmdIss::getObject()->addCmd(currentInput);
    if (!currentInput.isEmpty()) {
        historyCommand.back() = currentInput;
        historyCommand.emplace_back();
    }
    currentHistoryCommandIndex = historyCommand.size() - 1;
    currentInput = "";
    currentInputLeft = "";
    currentInputRight = "";
    std::cout << '>' << std::flush;
    emit loop();
}

void Shell::loop_() {
    unsigned char tmp = 0;
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
                if (!currentInputLeft.isEmpty()) {
                    currentInputLeft.removeLast();
                    std::cout << "\b \b";
                    std::cout << currentInputRight.data() << " \b";
                    for (size_t i = 0; i < currentInputRight.size(); i++)
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
                        for (size_t i = 0; i < currentInputLeft.size(); i++)
                            std::cout << "\b";
                        for (size_t i = 0; i < currentInput.size(); i++)
                            std::cout << " ";
                        for (size_t i = 0; i < currentInput.size(); i++)
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
                    for (size_t i = 0; i < currentInputLeft.size(); i++)
                        std::cout << "\b";
                    for (size_t i = 0; i < currentInput.size(); i++)
                        std::cout << " ";
                    for (size_t i = 0; i < currentInput.size(); i++)
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
                    if (!currentInputLeft.isEmpty()) {
                        char cilb = currentInputLeft[currentInputLeft.size() - 1];
                        currentInputLeft.removeLast();
                        currentInputRight = cilb + currentInputRight;
                        std::cout << '\b';
                    }
                } else if (tmp == 77) {
                    if (!currentInputRight.isEmpty()) {
                        char cirb = currentInputRight[0];
                        currentInputRight.removeFirst();
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
                            for (size_t i = 0; i < currentInputLeft.size(); i++)
                                std::cout << "\b";
                            for (size_t i = 0; i < currentInput.size(); i++)
                                std::cout << " ";
                            for (size_t i = 0; i < currentInput.size(); i++)
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
                        for (size_t i = 0; i < currentInputLeft.size(); i++)
                            std::cout << "\b";
                        for (size_t i = 0; i < currentInput.size(); i++)
                            std::cout << " ";
                        for (size_t i = 0; i < currentInput.size(); i++)
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
                            currentInputLeft.removeLast();
                            currentInputRight = cilb + currentInputRight;
                            std::cout << '\b';
                        }
                    } else if (tmp == 'C') { // 右
                        if (currentInputRight.size() != 0) {
                            char cirb = currentInputRight[0];
                            currentInputRight.removeFirst();
                            currentInputLeft += cirb;
                            std::cout << cirb;
                        }
                    }
                } else if (escReady) {
                    escReady = false;
                    if (tmp == 91)
                        terminalControlReady = true;
                    else if (((tmp != '\r') && (tmp != '\n')) && (' ' <= tmp && tmp <= '~')) {
                        if (tmpIsSave) {
                            currentHistoryCommandIndex = historyCommand.size() - 1;
                            tmpIsSave = false;
                        }
                        currentInputLeft.push_back((char)tmp);
                        std::cout << (char)tmp;
                        std::cout << currentInputRight.data();
                        for (size_t i = 0; i < currentInputRight.size(); i++)
                            std::cout << "\b";
                        std::cout << std::flush;
                    }
                } else if (tmp == 27) {
                    escReady = true;
#endif
            } else if (((tmp != '\r') && (tmp != '\n')) && (' ' <= tmp && tmp <= '~')) {
                if (tmpIsSave) {
                    currentHistoryCommandIndex = historyCommand.size() - 1;
                    tmpIsSave = false;
                }
                currentInputLeft.push_back((char)tmp);
                std::cout << (char)tmp;
                std::cout << currentInputRight.data();
                for (size_t i = 0; i < currentInputRight.size(); i++)
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
                for (size_t i = 0; i <= currentInputLeft.size(); i++)
                    std::cout << "\b";
                for (size_t i = 0; i <= currentInput.size(); i++)
                    std::cout << " ";
                for (size_t i = 0; i <= currentInput.size(); i++)
                    std::cout << "\b";
                addLogMutex.lock();
                empty = log.empty();
                addLogMutex.unlock();
                while (!empty) {
                    addLogMutex.lock();
                    LogInfo li = log.front();
                    log.pop_front();
                    addLogMutex.unlock();
                    if (li.eInfo) {
                        QByteArray rank;
                        switch (li.rank) {
                            case Logger::Info:
                                rank = "[I]";
                                break;
                            case Logger::Warning:
                                rank = "[W]";
                                break;
                            case Logger::Error:
                                rank = "[E]";
                                break;
                            default:
                                break;
                        }
                        QByteArray file;
                        {
                            auto i = li.file.end();
                            for (; i != li.file.begin(); i--)
                                if ((*i) == '/' || (*i) == '\\')break;
                            for (i++; i != li.file.end(); i++)
                                file.push_back(*i);
                        }
                        std::cout << li.time.data()
                                  << rank.data()
                                  << '[' << (void*)li.thread << ']'
                                  << '[' << file.data() << ':'
                                  << li.line << ']' << ' ';
                    }
#ifdef _WIN32
                    auto utf8ToGBK = [&](const QByteArray& utf8_str) {
                        int len = MultiByteToWideChar(CP_UTF8, 0, utf8_str.data(), -1, nullptr, 0);
                        auto wstr = new wchar_t[len + 1];
                        MultiByteToWideChar(CP_UTF8, 0, utf8_str.data(), -1, wstr, len);
                        len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
                        char *str = new char[len + 1];
                        WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, nullptr, nullptr);
                        QByteArray result(str);
                        delete[] wstr;
                        delete[] str;
                        return result;
                    };
                    std::cout << utf8ToGBK(li.data).data() << std::endl;
#elif __linux__
                    std::cout << li.data.data() << std::endl;
#endif
                    addLogMutex.lock();
                    empty = log.empty();
                    addLogMutex.unlock();
                }
                std::cout << '>' << currentInput.data();
                for (size_t i = 0; i < currentInputRight.size(); i++) std::cout << '\b';
                std::cout << std::flush;
            }
        }
    if ((tmp == '\r') || (tmp == '\n'))
        emit endLoop();
    else emit loop();
}