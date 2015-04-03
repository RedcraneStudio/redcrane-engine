
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

cmake_minimum_required(VERSION 3.1)
project(Survival_Games VERSION 0.0 LANGUAGES C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules/ ${CMAKE_MODULE_PATH})

include(data_target)

add_subdirectory(gl)
add_subdirectory(src)
add_subdirectory(assets)
