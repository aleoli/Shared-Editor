cmake_minimum_required(VERSION 3.13)
project(text_manager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

include_directories(${Qt5Widgets_INCLUDES})

list(
        APPEND LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)
list(REMOVE_DUPLICATES LIBRARIES)

add_executable(text_manager main.cpp TextEdit.cpp TextEdit.h TextManager.cpp TextManager.h)
target_link_libraries(text_manager ${LIBRARIES} )