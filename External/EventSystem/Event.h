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

#include<string>

namespace EventSystem {
    class EVENT_SYSTEM Listener;

    class EVENT_SYSTEM Event {                                   //事件
    public:

        explicit Event(const std::string&);

        [[maybe_unused]]
        void setAccepted(bool);

        [[nodiscard]]
        std::string getID() const;

        [[maybe_unused]] [[nodiscard]]
        bool isAccepted() const;

        [[maybe_unused]]
        void accept();

        [[maybe_unused]]
        void ignore();

        virtual ~Event();

    protected:
        void addListener(const Listener &);    //添加监听器
        void delListener(const Listener &);    //删除监听器
        void trigger();                        //触发事件

        std::string ID;
        bool m_accept;
    };
}
