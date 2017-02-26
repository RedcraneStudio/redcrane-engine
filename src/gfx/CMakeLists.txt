
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_subdirectory(extra)

# These are all the opengl sources.
set(RED_CRANE_GFX_GL_SOURCES
  gl/driver.cpp
  gl/mesh.cpp
  gl/texture.cpp
  gl/shader.cpp
  gl/buffer.cpp
  gl/common.cpp
  gl/framebuffer.cpp)

# These are all the null implementations of the ABCs.
set(RED_CRANE_GFX_NULL_SOURCES null/driver.cpp null/mesh.cpp)

add_library(gfxlib STATIC ${RED_CRANE_GFX_GL_SOURCES} camera.cpp ishader.cpp
                          imesh.cpp itexture.cpp mesh_chunk.cpp common.cpp
                          mesh_data.cpp immediate_renderer.cpp scene.cpp
                          deferred.cpp asset_render.cpp)

# Link to our extension loader (glad).
target_link_libraries(gfxlib engine_gl commonlib assetslib ${GLFW_LIBRARY}
                             ${Boost_SYSTEM_LIBRARY}
                             ${Boost_FILESYSTEM_LIBRARY})

# We need this for boost::optional.
target_include_directories(gfxlib PUBLIC ${GLFW_INCLUDE_DIR}
                                         ${GLM_INCLUDE_DIR}
                                         ${Boost_INCLUDE_DIRS})
