
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

include_directories(${CMAKE_SOURCE_DIR})

find_package(LibPNG REQUIRED)
find_package(LibUV REQUIRED)
add_subdirectory(common)
add_subdirectory(io)

find_package(GLFW REQUIRED)
add_subdirectory(gfx)

add_subdirectory(fps)

#find_package(Boost REQUIRED COMPONENTS filesystem)
#add_subdirectory(assets)

find_package(Freetype REQUIRED)
add_subdirectory(ui)

add_subdirectory(map)

add_executable(strategy main.cpp)
target_link_libraries(strategy gfxlib commonlib uilib maplib)

find_package(Bullet REQUIRED)

add_executable(fps_exe main_fps.cpp)
target_link_libraries(fps_exe gfxlib commonlib fpslib)
target_link_libraries(fps_exe ${BULLET_LIBRARIES})

target_include_directories(fps_exe PUBLIC ${BULLET_INCLUDE_DIRS})

add_subdirectory(tests)
