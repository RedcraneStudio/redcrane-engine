
# Copyright (C) 2014 Luke San Antonio
# All rights reserved.

include_directories(${CMAKE_SOURCE_DIR})

find_package(LibPNG REQUIRED)
find_package(LibUV REQUIRED)
add_subdirectory(common)

find_package(GLFW REQUIRED)
add_subdirectory(gfx)

add_executable(survive main.cpp)
target_link_libraries(survive gfxlib)
