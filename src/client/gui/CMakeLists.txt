cmake_minimum_required(VERSION 3.10)
project(client)

include(../../../cmake/qt_libraries.cmake)

set(CMAKE_CXX_STANDARD 17)

include_directories(..)
include_directories(../..)
include_directories(account)
include_directories(browser)
include_directories(editor)
include_directories(landing)
include_directories(login)
include_directories(other)
include_directories(registration)

file(GLOB src_files
    "*.h"
    "*.cpp"
    "../*.h"
    "../*.cpp"
    "../../*.h"
    "../../*.cpp"
    "*.ui"
    "*.qrc"
    "account/*.h"
    "account/*.cpp"
    "account/*.ui"
    "browser/*.h"
    "browser/*.cpp"
    "browser/*.ui"
    "editor/*.h"
    "editor/*.cpp"
    "editor/*.ui"
    "landing/*.h"
    "landing/*.cpp"
    "landing/*.ui"
    "login/*.h"
    "login/*.cpp"
    "login/*.ui"
    "other/*.h"
    "other/*.cpp"
    "other/*.ui"
    "registration/*.h"
    "registration/*.cpp"
    "registration/*.ui"
)

add_executable(client ${src_files})

target_link_libraries(client ${LIBRARIES} pthread)
