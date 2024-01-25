cmake_minimum_required(VERSION 3.14)
set(projectName Aioaeichi)
project(${projectName} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(UNIX)
    set(platformSrc Aioaeichi/tools/conio.cpp)
endif ()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(./${projectName})
include_directories(./include)

link_directories(libs)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

add_executable(
        ${projectName}
        main.cpp
        Aioaeichi/Shell/Shell.cpp
        Aioaeichi/PluginsManager/PluginsManager.cpp
        ${platformSrc}
)

target_link_libraries(
        ${projectName}
        Qt${QT_VERSION_MAJOR}::Core
        Screw
        )
