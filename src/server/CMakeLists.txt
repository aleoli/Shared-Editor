cmake_minimum_required(VERSION 3.10)
project(server)

include(../../cmake/qt_libraries.cmake)

set(CMAKE_CXX_STANDARD 17)

include_directories(..)
include_directories(persistence)

file(GLOB src_files
    "*.h"
    "*.cpp"
    "../*.h"
    "../*.cpp"
    "io_thread/*.h"
    "io_thread/*.cpp"
)

add_executable(server ${src_files})

target_link_libraries(server ${LIBRARIES} pthread)
