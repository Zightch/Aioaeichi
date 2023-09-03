cmake_minimum_required(VERSION 3.14)
set(projectName Aioaeichi)
project(${projectName} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (WIN32)
        set(CMAKE_PREFIX_PATH "E:\\Qt\\6.5.2\\mingw_64\\lib\\cmake")
        set(libs)
elseif(UNIX)
        set(CMAKE_PREFIX_PATH "/opt/Qt/6.5.2/gcc_64/lib/cmake")
        set(libs dl)
endif ()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(./${projectName})

link_directories(libs)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

add_executable(
        ${projectName}
        main.cpp
        Aioaeichi/CmdIss/CmdIss.cpp
        Aioaeichi/Shell/Shell.cpp
        Aioaeichi/Logger/Logger.cpp
        Aioaeichi/PluginsManager/PluginsManager.cpp
        Aioaeichi/DLLLoader/DLLLoader.cpp
        )

target_link_libraries(
        ${projectName}
        Qt${QT_VERSION_MAJOR}::Core
        ${libs}
        )
