cmake_minimum_required(VERSION 3.10)
project(stubClient)

include(../../../cmake/qt_libraries.cmake)

set(CMAKE_CXX_STANDARD 17)

include_directories(..)
include_directories(../..)

file(GLOB src_files
    "*.h"
    "*.cpp"
    "../*.h"
    "../*.cpp"
    "../../*.h"
    "../../*.cpp"
    "*.ui"
    "*.qrc"
)

add_executable(stubClient ${src_files})

target_link_libraries(stubClient ${LIBRARIES} pthread)
