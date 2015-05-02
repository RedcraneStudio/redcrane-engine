
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(commonlib STATIC log.cpp translate.cpp json.cpp animation.cpp
                             texture_load.cpp software_texture.cpp
                             texture_composite.cpp mesh_load.cpp
                             software_mesh.cpp)
target_link_libraries(commonlib ${LIBUV_LIBRARIES})

target_link_libraries(commonlib ${LIBPNG_LIBRARIES})
target_include_directories(commonlib PUBLIC ${LIBPNG_INCLUDE_DIRS})
