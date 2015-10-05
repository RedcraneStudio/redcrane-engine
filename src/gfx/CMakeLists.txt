
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_subdirectory(support)

# These are all the opengl sources.
set(RED_CRANE_GFX_GL_SOURCES gl/driver.cpp gl/mesh.cpp gl/texture.cpp
                             gl/shader.cpp gl/common.cpp)

# These are all the null implementations of the ABCs.
set(RED_CRANE_GFX_NULL_SOURCES null/driver.cpp null/mesh.cpp)

add_library(gfxlib STATIC ${RED_CRANE_GFX_GL_SOURCES} camera.cpp shader.cpp
                          ${RED_CRANE_GFX_NULL_SOURCES} idriver_ui_adapter.cpp
                          mesh.cpp mesh_chunk.cpp mesh_data.cpp
                          immediate_renderer.cpp)

# Link to our extension loader (glad).
target_link_libraries(gfxlib engine_gl)

# This is for our simple little asynchronous logging library.
target_link_libraries(gfxlib commonlib)

# We need this for boost::optional.
target_include_directories(gfxlib PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(gfxlib ${GLFW_LIBRARY})
target_include_directories(gfxlib PUBLIC ${GLFW_INCLUDE_DIR})

target_include_directories(gfxlib PUBLIC ${GLM_INCLUDE_DIR})
