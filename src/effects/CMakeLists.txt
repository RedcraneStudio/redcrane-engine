
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(effectslib STATIC envmap.cpp water.cpp)

target_link_libraries(effectslib PUBLIC uselib)
