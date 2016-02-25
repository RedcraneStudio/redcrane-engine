
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(fpslib STATIC camera_controller.cpp)

target_link_libraries(fpslib gfxlib)
