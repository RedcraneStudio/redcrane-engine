
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(boatlib STATIC boat.cpp boat_def.cpp)

target_link_libraries(boatlib commonlib gfxlib uselib)
