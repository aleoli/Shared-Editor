project(SharedEditor)

cmake_minimum_required (VERSION 3.10)

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Release FORCE)
endif()

set (CMAKE_CXX_FLAGS "-O3")
set (CMAKE_CXX_FLAGS_RELEASE "-O3")
set (CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
