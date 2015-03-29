
# Copyright (C) 2014 Luke San Antonio
# All rights reserved.

add_library(gfxlib STATIC mesh.cpp pipeline.cpp program.cpp texture.cpp
                          material.cpp)

target_link_libraries(gfxlib survival_games_gl)

# This is for our simple little asynchronous logging library.
target_link_libraries(gfxlib commonlib)

target_link_libraries(gfxlib ${GLFW_LIBRARY})
target_include_directories(gfxlib PUBLIC ${GLFW_INCLUDE_DIR})

target_link_libraries(gfxlib ${LIBPNG_LIBRARIES})
target_include_directories(gfxlib PUBLIC ${LIBPNG_INCLUDE_DIRS})
