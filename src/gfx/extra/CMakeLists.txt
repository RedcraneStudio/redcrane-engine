
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(gfxextralib STATIC
  load_wavefront.cpp
  mesh_conversion.cpp
  allocate.cpp
  format.cpp
  texture_load.cpp
  write_data_to_mesh.cpp
  scoped_shader_lock.cpp
  generate_aabb.cpp
  render_normals.cpp
  proj_grid.cpp)

target_link_libraries(gfxextralib gfxlib commonlib)

# We need this for boost::optional.
target_include_directories(gfxextralib PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(gfxextralib ${LIBPNG_LIBRARIES})
target_include_directories(gfxextralib PUBLIC ${LIBPNG_INCLUDE_DIRS})
