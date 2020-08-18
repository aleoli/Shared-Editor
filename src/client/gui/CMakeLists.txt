cmake_minimum_required(VERSION 3.10)
project(client)

include(../../../cmake/qt_libraries.cmake)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(..)
include_directories(../..)
include_directories(docsbrowser)
include_directories(edit)
include_directories(landing)
include_directories(login)
include_directories(registration)
include_directories(texteditor)
include_directories(dialogs)

file(GLOB src_files
    "*.h"
    "*.cpp"
    "res/*.qrc"
    "../*.h"
    "../*.cpp"
    "../../*.h"
    "../../*.cpp"
    "ui/*.ui"
    "ui/resources/*.qrc"
    "docsbrowser/*.h"
    "docsbrowser/*.cpp"
    "edit/*.h"
    "edit/*.cpp"
    "landing/*.h"
    "landing/*.cpp"
    "login/*.h"
    "login/*.cpp"
    "registration/*.h"
    "registration/*.cpp"
    "texteditor/*.h"
    "texteditor/*.cpp"
    "dialogs/*.h"
    "dialogs/*.cpp"
)

add_executable(client ${src_files} docsbrowser/docwidget.cpp docsbrowser/docwidget.h docsbrowser/docwidget_folder.cpp docsbrowser/docwidget_folder.h)

target_link_libraries(client ${LIBRARIES} pthread)
