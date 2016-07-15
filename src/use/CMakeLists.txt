
# Copyright (C) 2014 Luke San Antonio
# All rights reserved.

add_library(uselib mesh.cpp mesh_cache.cpp texture.cpp)
target_link_libraries(uselib PUBLIC commonlib gfxlib gfxsupportlib assetslib)
