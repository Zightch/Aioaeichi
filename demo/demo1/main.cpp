#include "Logger.h"
#include "CmdIss.h" // 引入命令分发器头文件

// 定义命令处理器, 参数分别为cmd, args, proc, accept
CmdIss::CmdProc cmdProc([](const QByteArray &cmd, const QByteArrayList &args, bool &proc, bool &accept) {
    if (cmd == "demo1") { // 如果cmd == "demo1"
        LOG(Info, "Hello, Aioaeichi");
        proc = true; // 告诉CmdIss该命令已被处理
    } else if (cmd == "help") {
        // 帮助命令的输出并不需要时间等级等信息
        LOG_NO_INFO("demo1 打印Hello, Aioaeichi");
        proc = true; // 告诉CmdIss该命令已被处理
        accept = false; // 同时保证命令不会被当前处理器截止
    } else
        accept = false; // 在没有任何命令处理时保证命令不会被当前处理器截止
});

extern "C"
{
void init() {}
void start() {
    // 使用命令分发器的单例对象注册咱们刚刚写的命令处理器
    CmdIss::getObject()->addCmdCallBack(&cmdProc);
}
void unload() {}
}
