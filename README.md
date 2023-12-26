[mirrors](http://101.43.118.146:19168/Zightch/Aioaeichi/)

# Aioaeichi
`Aioaeichi`是一个用C++基于QT框架封装的跨平台的虚拟控制台

## ✨ 特性
* 支持windows和linux操作系统  
* 只需要少量的依赖库和资源，它可以快速地启动和关闭  
* 重写了标准输入以及提供了一个完善的命令分发机制，它可以让用户方便地查看日志信息和输入命令  
* 解决了开发时搭建一个简易的控制台而产生的烦恼  

现在你将拥有Aioaeichi, 请继续阅读, 去提升你的开发便捷性  

## ⚙ 功能
* 日志打印
  * 打印输出内容: `日期`, `日志等级`, `调用日志打印所在线程`, `调用日志打印所在文件及行号`, `日志内容`
  * 允许打印不带信息的日志内容
  * 多线程安全
* 用户命令读取
  * 通过重写标准输入实现与日志打印不相冲突, 不会出现输出内容接在输入内容之后的情况
  * 带有完善的命令分发机制
  * 支持历史命令翻阅
* 插件(允许自己编写插件实现不同功能)
  * 你的构建生成可以更改为`动态库`(.dll | .so)了, Aioaeichi将会将你的动态库作为`插件`, 在Aioaeichi启动时加载会加载你的插件
  * 你的插件中也是需要包含插件入口点, 在插件中你只需要注册命令回调, 和调用日志打印即可

## ※ 注意
* Aioaeichi并不是一个真正的控制台, 并不能通过Aioaeichi去执行其他命令
* Aioaeichi只是基于QT的一个虚拟控制台的封装
* Aioaeichi只解决开发应用程序需要搭建控制台的问题

## 📞 联系方式
* QQ号: 2166825850
* 邮箱: Zightch@163.com

## 🔑 许可证和版权声明
Aioaeichi是一个开源的软件，它遵循[`MIT许可证`](License)的条款和条件发布。你可以自由地使用、修改和分发Aioaeichi，但是你需要保留原作者的版权声明和许可证声明。你对Aioaeichi的使用和修改不承担任何责任，原作者也不对Aioaeichi的功能和性能做任何保证。

---
# ⚡ 快速入门
准备好开始使用了吗?  
从现在开始本文章假定你有一定的QT以及cmake等基础知识

## 构建Aioaeichi
如果你是直接从`发布`中下载已经构建好的, 本步骤可以跳过  
>`发布`中的构建产物均为`QT6.6.0` `g++`编译  
>如果你使用的`msvc`或者`其他版本QT`, 那么该步骤将不能跳过  
>你需要自行编译`msvc`或者`其他版本QT`的构建产物

1. 你需要修改[`CMakeLists.txt`](CMakeLists.txt)文件中的`第8行`和`第12行`  
  以配置`CMAKE_PREFIX_PATH`变量, 确保能够找到你的QT SDK  
  当然, 将此变量写入`系统环境变量`也是不错的选择
2. 利用cmake生成makefile或其他构建工具的`build`目录, 方式不限
3. 构建项目, 复制依赖到你的`build`目录下, 其中依赖包括`Qt6Core.dll/so`, 方式不限

同时, `Aioaeichi`依赖于`Screw`  
这也意味着你可能也要编译一下`Screw`  
编译步骤与`Aioaeichi`相同, `Screw`源码在`Screw`分支中

## 创建插件项目
1.  新建文件夹, 并新建`CMakeLists.txt`文件, 输入如下内容
    <details>
      <summary>展开以查看内容</summary>

    ```
    cmake_minimum_required(VERSION 3.21)
    set(projectName demo)
    project(${projectName})
    
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)
    
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    
    if (WIN32)
        set(CMAKE_PREFIX_PATH "E:\\Qt\\6.6.0\\mingw_64\\lib\\cmake")
    elseif(UNIX)
        set(CMAKE_PREFIX_PATH "/opt/Qt/6.6.0/gcc_64/lib/cmake")
    endif ()
    
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    
    link_directories(libs)
    
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
    
    include_directories(include)
    include_directories(./${projectName})
    
    add_library(
        ${projectName}
        SHARED
        main.cpp
    )
    
    target_link_libraries(
        ${projectName}
        Qt${QT_VERSION_MAJOR}::Core
        Screw
    )
    ```

    </details>

    ※ 此刻同样要注意`第15行`和`第17行`的`CMAKE_PREFIX_PATH`变量以及QT版本要与Aioaeichi对应
2.  从`发布`中下载`include & lib.zip`
3.  解压`include & lib.zip`到项目文件夹根目录中, 其中`include`为包含文件, `libs`为Screw库文件
4.  新建`main.cpp`, 输入如下内容
    ```C++
    #include "Screw/Logger.h"

    #ifdef _WIN32
    #define DEMO extern "C" __declspec(dllexport)
    #elif __linux__
    #define DEMO extern "C"
    #endif
    
    DEMO void init() {}
    DEMO void start() {
        LOG(Info, "Hello, Aioaeichi");
    }
    DEMO void unload() {}
    ```
5.  小结, 如果你的操作没有问题, 你的插件项目目录应该如下所示
    ```
    ├──include
    │  └──Screw
    │     ├──CmdIss.h
    │     └──Logger.h
    ├──libs
    │  └──libScrew.dll|so
    ├──main.cpp
    └──CmakeLists.txt
    ```
6.  利用你擅长的方式构建项目
7.  将你构建的或你下载的`Aioaeichi`放到你的构建目录下
8.  运行Aioaeichi, 即可在控制台看到如下内容
    ```
    [2023-08-31 17:34:32][I][0x2][PluginsManager.cpp:50] 开始加载插件
    [2023-08-31 17:34:32][I][0x2][PluginsManager.cpp:89] 已加载libdemo.dll
    [2023-08-31 17:34:32][I][0x2][PluginsManager.cpp:103] 开始初始化插件
    [2023-08-31 17:34:32][I][0x2][PluginsManager.cpp:108] 正在初始化libdemo.dll
    [2023-08-31 17:34:32][I][0x2][PluginsManager.cpp:110] libdemo.dll初始化完成
    [2023-08-31 17:34:32][I][0x2][PluginsManager.cpp:126] 开始启用插件
    [2023-08-31 17:34:32][I][0x2][PluginsManager.cpp:131] 正在启用libdemo.dll
    [2023-08-31 17:34:32][I][0x2][main.cpp:8] Hello, Aioaeichi
    [2023-08-31 17:34:32][I][0x2][PluginsManager.cpp:133] libdemo.dll已启用
    [2023-08-31 17:34:32][I][0x2][PluginsManager.cpp:159] 已成功加载1个插件
    >
    ```
    当你看到这些就意味着你大功告成了
9.  在`>`之后输入`exit`即可退出程序

完整项目这里查看[./demo/demo0](./demo/demo0)

## 最后
都看到这里了, 不妨点个star支持一下  
想要深入了解Aioaeichi的使用可以翻阅[这篇文档](advanced.md)
