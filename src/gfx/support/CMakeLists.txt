
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(gfxsupportlib STATIC load_wavefront.cpp software_texture.cpp
                                 texture_composite.cpp texture_load.cpp
                                 location_cache.cpp scoped_shader_lock.cpp
                                 mesh_conversion.cpp write_data_to_mesh.cpp
                                 generate_aabb.cpp)
target_link_libraries(gfxsupportlib gfxlib commonlib)

target_link_libraries(gfxsupportlib ${LIBPNG_LIBRARIES})
target_include_directories(gfxsupportlib PUBLIC ${LIBPNG_INCLUDE_DIRS})
