cmake_minimum_required(VERSION 3.10)
project(client)

include(../../cmake/qt_libraries.cmake)

set(CMAKE_CXX_STANDARD 17)

include_directories(..)
include_directories(test) # da togliere

file(GLOB src_files
    "*.h"
    "*.cpp"
    "../*.h"
    "../*.cpp"
    "test/*.h"
    "test/*.cpp"
)

add_executable(client ${src_files} test/resources.qrc)

target_link_libraries(client ${LIBRARIES} pthread)
