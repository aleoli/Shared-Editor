cmake_minimum_required(VERSION 3.10)
project(client)

include(../../cmake/qt_libraries.cmake)

set(CMAKE_CXX_STANDARD 17)

include_directories(..)
include_directories(test) # da togliere
include_directories(stub) # da togliere

file(GLOB src_files
    "*.h"
    "*.cpp"
    "../*.h"
    "../*.cpp"
    "*.ui"
    "*.qrc"
)

file(GLOB test_files
    "test/*.h"
    "test/*.cpp"
)

file(GLOB stub_files
    "stub/*.h"
    "stub/*.cpp"
    "stub/*.ui"
    "stub/*.qrc"
)
add_executable(client ${src_files} ${test_files} ${stub_files})

target_link_libraries(client ${LIBRARIES} pthread)
