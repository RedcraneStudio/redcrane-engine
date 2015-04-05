
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

include_directories(${CMAKE_SOURCE_DIR})

find_package(LibPNG REQUIRED)
find_package(LibUV REQUIRED)
add_subdirectory(common)
add_subdirectory(io)

find_package(GLFW REQUIRED)
add_subdirectory(gfx)

find_package(Boost REQUIRED COMPONENTS filesystem)
add_subdirectory(assets)

add_executable(strat main.cpp mesh.cpp texture.cpp scene_node.cpp
                     mesh_container.cpp)
target_link_libraries(strat gfxlib)
