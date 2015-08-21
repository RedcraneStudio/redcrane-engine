
# This file not part of open-simplex-noise-in-c.

# Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
# All rights reserved.

add_library(opensimplex STATIC open-simplex-noise.c)

target_include_directories(opensimplex PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
