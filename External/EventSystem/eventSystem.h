#pragma once

#ifdef _WIN32
#ifdef EVENT_SYSTEM_EXPORT
#define EVENT_SYSTEM __declspec(dllexport)
#else
#define EVENT_SYSTEM __declspec(dllimport)
#endif
#elif __linux__
#define EVENT_SYSTEM
#endif

#include "Listener.h"

namespace EventSystem {
    [[maybe_unused]] void EVENT_SYSTEM trigger(Event &);//触发事件
    [[maybe_unused]] void EVENT_SYSTEM trigger(Event *);//触发事件
    [[maybe_unused]] void EVENT_SYSTEM deleteEvent(const std::string&);//删除某一事件
    [[maybe_unused]] void EVENT_SYSTEM deleteEvent(Event *);//删除某一事件
    [[maybe_unused]] void EVENT_SYSTEM deleteEvent(Event &);//删除某一事件
    [[maybe_unused]] void EVENT_SYSTEM deleteListener(const std::string&);//删除某一监听器
    [[maybe_unused]] void EVENT_SYSTEM deleteListener(Listener *);//删除某一监听器
    [[maybe_unused]] void EVENT_SYSTEM deleteListener(Listener &);//删除某一监听器
    [[maybe_unused]] void EVENT_SYSTEM clearEvent();//清空所有事件
    [[maybe_unused]] void EVENT_SYSTEM clearListener();//清空所有监听器
}
