cmake_minimum_required(VERSION 3.10)
project(server)

include(../../cmake/qt_libraries.cmake)

set(CMAKE_CXX_STANDARD 17)

include_directories(..)
include_directories(persistence)

find_package(Qt5Core REQUIRED)
find_package(Qt5Sql REQUIRED)

file(GLOB src_files
    "*.h"
    "*.cpp"
    "../*.cpp"
    "../*.h"
)

list(
  APPEND LIBRARIES
  Qt5::Core
  Qt5::Sql
)
list(REMOVE_DUPLICATES LIBRARIES)

add_executable(server ${src_files})

target_link_libraries(server ${LIBRARIES} pthread)
