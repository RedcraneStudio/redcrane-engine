
# Copyright (C) 2014 Luke San Antonio
# All rights reserved.

add_library(waterlib grid.cpp)
target_link_libraries(waterlib PUBLIC genlib commonlib gfxlib gfxsupportlib
                                      opensimplex)
