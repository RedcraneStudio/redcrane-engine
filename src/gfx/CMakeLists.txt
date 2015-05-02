
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

set(SURVIVE_GFX_GL_SOURCES gl/driver.cpp gl/standard_shader.cpp gl/program.cpp
                           gl/hud_shader.cpp gl/mesh.cpp)
add_library(gfxlib STATIC ${SURVIVE_GFX_GL_SOURCES}
                          material.cpp object.cpp camera.cpp scene_node.cpp)

target_link_libraries(gfxlib engine_gl)

# This is for our simple little asynchronous logging library.
target_link_libraries(gfxlib commonlib)

target_link_libraries(gfxlib ${GLFW_LIBRARY})
target_include_directories(gfxlib PUBLIC ${GLFW_INCLUDE_DIR})

target_link_libraries(gfxlib ${LIBPNG_LIBRARIES})
target_include_directories(gfxlib PUBLIC ${LIBPNG_INCLUDE_DIRS})
