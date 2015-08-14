
# Copyright (C) 2014 Luke San Antonio
# All rights reserved.

add_library(stratlib event.cpp map.cpp player_state.cpp structure.cpp
                     terrain.cpp water.cpp)
target_link_libraries(stratlib gfxlib gfxsupportlib collisionlib)
