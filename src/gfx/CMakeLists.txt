
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_subdirectory(support)

set(SURVIVE_GFX_GL_SOURCES gl/driver.cpp gl/mesh.cpp gl/texture.cpp
                           gl/shader.cpp gl/common.cpp)
add_library(gfxlib STATIC ${SURVIVE_GFX_GL_SOURCES} camera.cpp shader.cpp
                          idriver_ui_adapter.cpp mesh.cpp mesh_chunk.cpp
                          mesh_data.cpp)

target_link_libraries(gfxlib engine_gl)

# This is for our simple little asynchronous logging library.
target_link_libraries(gfxlib commonlib)

target_link_libraries(gfxlib ${GLFW_LIBRARY})
target_include_directories(gfxlib PUBLIC ${GLFW_INCLUDE_DIR})
