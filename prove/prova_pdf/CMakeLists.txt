cmake_minimum_required(VERSION 3.13)
project(prova_pdf)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)

include_directories(${Qt5Widgets_INCLUDES})

list(
        APPEND LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::PrintSupport
)
list(REMOVE_DUPLICATES LIBRARIES)

add_executable(prova_pdf main.cpp)
target_link_libraries(prova_pdf ${LIBRARIES} )