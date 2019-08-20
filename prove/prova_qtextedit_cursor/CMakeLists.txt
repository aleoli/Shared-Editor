cmake_minimum_required(VERSION 3.14)
project(prova_qtextedit)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

list(APPEND LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets)
list(REMOVE_DUPLICATES LIBRARIES)


add_executable(prova_qtextedit main.cpp TextEdit.cpp TextEdit.h Cursor.cpp Cursor.h)
target_link_libraries(prova_qtextedit ${LIBRARIES})