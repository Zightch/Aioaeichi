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
        set(CMAKE_PREFIX_PATH "D:\\Qt\\6.5.2\\mingw_64\\lib\\cmake")
        set(libs Aioaeichi)
elseif(UNIX)
        set(CMAKE_PREFIX_PATH "/opt/Qt/6.5.2/gcc_64/lib/cmake")
        set(libs Aioaeichi)
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
        ${libs}
        )