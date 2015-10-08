
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

cmake_minimum_required(VERSION 3.1)
project(Red_Crane_Engine VERSION 0.0 LANGUAGES C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules/ ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_FIND_FRAMEWORK LAST)

include(data_target)

add_subdirectory(gl)
add_subdirectory(opensimplex)
add_subdirectory(mods)
add_subdirectory(src)
add_subdirectory(assets)
add_subdirectory(tests)
