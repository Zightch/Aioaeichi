# 深入了解Aioaeichi
很好, 既然你看到了这里说明你对Aioaeichi是感兴趣的  
那么接下来这篇文章将带领你认识Aioaeichi更多的用法以及特性

## 用户命令监听
在前面的文章中我们已经实现了在插件被加载时向屏幕输出`Hello, Aioaeichi`  
那么在程序运行期间, 用户可以通过控制台输入指令  
输入后的指令会经过Aioaeichi的命令分发器进行命令的传递  
此时你只需要将你的命令处理器注册给命令分发器即可  
在命令处理器中你可以判定传进来的命令是否时你想要的
```C++
/* main.cpp */
#include "Logger.h"
#include "CmdIss.h" // 引入命令分发器头文件

// 定义命令处理器, 参数分别为cmd, args, proc, accept
CmdIss::CmdProc cmdProc([](const QByteArray &cmd, const QByteArrayList &args, bool &proc, bool &accept) {
    if (cmd == "demo1") { // 如果cmd == "demo1"
        LOG(Info, "Hello, Aioaeichi");
    }
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
```
构建后运行Aioaeichi, 你将能看到如下内容
```
[2023-08-31 23:18:13][I][0x2][PluginsManager.cpp:50] 开始加载插件
[2023-08-31 23:18:13][I][0x2][PluginsManager.cpp:89] 已加载libdemo.dll
[2023-08-31 23:18:13][I][0x2][PluginsManager.cpp:103] 开始初始化插件
[2023-08-31 23:18:13][I][0x2][PluginsManager.cpp:108] 正在初始化libdemo.dll
[2023-08-31 23:18:13][I][0x2][PluginsManager.cpp:110] libdemo.dll初始化完成
[2023-08-31 23:18:13][I][0x2][PluginsManager.cpp:126] 开始启用插件
[2023-08-31 23:18:13][I][0x2][PluginsManager.cpp:131] 正在启用libdemo.dll
[2023-08-31 23:18:13][I][0x2][PluginsManager.cpp:133] libdemo.dll已启用
[2023-08-31 23:18:13][I][0x2][PluginsManager.cpp:159] 已成功加载1个插件
>
```
此时我们在`>`后面输入命令`demo1`, 你将会看到如下内容
```
>demo1
[2023-08-31 23:19:52][I][0x2][main.cpp:7] Hello, Aioaeichi
[2023-08-31 23:19:52][I][0x2][CmdIss.cpp:76] "demo1"未知的命令
>
```
此时就发生问题了, 我们的`demo1`命令命名已经处理掉了, 为什么CmdIss仍然提示"demo1位置的命令"  
因为CmdIss并不知道你的命令到底有无被处理, 如果有注册的命令处理器就判定为每次分发的命令一定是被处理的这并不合适  
所以在参数中提供了一个`bool &proc`参数来表示命令是否被处理, 也就是说我们需要在LOG之后写一句`proc = true;`表示该命令被咱们处理了
```C++
if (cmd == "demo1") { // 如果cmd == "demo1"
    LOG(Info, "Hello, Aioaeichi");
    proc = true;
}
```
然后我们再次运行输入demo1命令就不会提示未知的命令了  
那我们解释完了cmd, proc这两个参数之后, 还剩下args, accept这两个参数  
args顾名思义就是`命令的`参数列表, 参数中不包括命令本身  
例如命令`abc 123 def`, 其中cmd = "abc", args = ["123", "def"]  
accept参数的作用是表示该命令会不会被截止到当前的`命令处理器`  
此参数默认为true, 也就是即使你的命令处理器中什么也没有做, 当前命令都会截止到你的处理器下, 不再先后传递  
你现在可以做个实验, 就用刚才我们写的代码, 运行后输入`exit`命令, 看看他会呈现怎样的效果  
如果不出意外的话, CmdIss会提示你`exit未知的命令`  
这是因为`exit`命令最为特殊, 他是关闭整个程序的唯一命令, 因此该命令会在所有注册的处理器后被处理  
但是当exit命令分发到你的处理器的时候被截止了, 并且proc不是true, 所以最后会提示`exit未知的命令`  
那么解决方法就是在你的if之后`else accept = false;`即可
```C++
if (cmd == "demo1") { // 如果cmd == "demo1"
    LOG(Info, "Hello, Aioaeichi");
    proc = true; // 告诉CmdIss该命令已被处理
} else
    accept = false; // 在没有任何命令处理时保证命令不会被当前处理器截止
```
现在再次构建运行将会一切正常  
那你可能就会有疑问, 为什么非要设计一个accept来控制截止, 只需要判断proc不就好了  
因为还有一种命令情况叫做`help`命令  
我们需要保证help命令被处理的情况下要继续向后传递, 这样才能将所有处理器中的help的处理都运行到位  
当然了不止help命令, 如果你的某些命令的处理方式类似help, 也可以用此方法
```C++
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
```
至此, 用户命令监听的所有内容全部解释完毕
