
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(effectslib STATIC envmap.cpp ocean.cpp)

target_link_libraries(effectslib PUBLIC uselib waterlib gfxlib gfxsupportlib
                                        boatlib)
