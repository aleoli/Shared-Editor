cmake_minimum_required(VERSION 3.13)
project(QtSQL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)
#find_package(Qt5Gui REQUIRED)
#find_package(Qt5Widgets REQUIRED)
#find_package(Qt5Charts REQUIRED)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")


#include_directories(${Qt5Widgets_INCLUDES})


list(
        APPEND LIBRARIES
        Qt5::Core
        Qt5::Sql
#        Qt5::Gui
#        Qt5::Widgets
#        Qt5::Charts
)
list(REMOVE_DUPLICATES LIBRARIES)


add_executable(QtSQL main.cpp)
target_link_libraries(QtSQL ${LIBRARIES} )
