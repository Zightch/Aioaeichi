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

#include"Event.h"
#include<functional>

namespace EventSystem {
    class EVENT_SYSTEM Listener {                              //监听器
    public:

        [[maybe_unused]]
        explicit Listener(const std::string &);

        explicit Listener(const std::string &, const std::function<void(Event *)>&);  //初始化ID和要运行的函数

        [[maybe_unused]]
        void setFunction(const std::function<void(Event *)>&);                      //设置要运行的函数
        [[maybe_unused]] [[nodiscard]]
        std::string getID() const;                           //获取ID
        [[maybe_unused]]
        Listener &listenEvent(Event &);                //监听事件(添加事件)
        [[maybe_unused]]
        Listener &unListenEvent(Event &);              //停止监听事件(删除事件)
        [[maybe_unused]]
        Listener &listenEvent(Event *);                //监听事件(添加事件)
        [[maybe_unused]]
        Listener &unListenEvent(Event *);              //停止监听事件(删除事件)
        [[maybe_unused]]
        Listener &listenEvent(const std::string &);            //监听事件(添加事件)
        [[maybe_unused]]
        Listener &unListenEvent(const std::string &);          //停止监听事件(删除事件)

        virtual ~Listener();

    protected:
        std::string ID;
    };
}
