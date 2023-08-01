cmake_minimum_required(VERSION 3.14)
set(projectName Aioaeichi)
project(${projectName} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (WIN32)
        set(CMAKE_PREFIX_PATH "D:\\Qt\\6.5.1\\mingw_64\\lib\\cmake")
        set(libs EventSystem Screw)
        add_compile_options("$<$<C_COMPILER_ID:GNU>:-fexec-charset=GBK>")
        add_compile_options("$<$<CXX_COMPILER_ID:GNU>:-fexec-charset=GBK>")
        add_compile_options("$<$<C_COMPILER_ID:GNU>:-finput-charset=utf-8>")
        add_compile_options("$<$<CXX_COMPILER_ID:GNU>:-finput-charset=utf-8>")
elseif(unix)
        set(CMAKE_PREFIX_PATH "/opt/Qt/6.5.1/gcc_64/lib/cmake")
        set(libs EventSystem Screw dl)
endif ()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(./External)
include_directories(./${projectName})

link_directories(libs)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

add_executable(
        ${projectName}
        main.cpp
        Aioaeichi/ConsoleCommand/ConsoleCommand.cpp
        Aioaeichi/CommandProcessing/CommandProcessing.cpp
        Aioaeichi/PluginsManager/PluginsManager.cpp
        Aioaeichi/DLLLoader/DLLLoader.cpp
        )

target_link_libraries(
        ${projectName}
        Qt${QT_VERSION_MAJOR}::Core
        ${libs}
        )
