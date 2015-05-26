
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(maplib json_structure.cpp map.cpp structure.cpp water.cpp)
target_link_libraries(maplib gfxlib commonlib)
