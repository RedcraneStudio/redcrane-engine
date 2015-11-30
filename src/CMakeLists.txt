
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

include_directories(${CMAKE_SOURCE_DIR})

find_package(LibPNG REQUIRED)
find_package(LibUV REQUIRED)
find_package(GLM REQUIRED)
add_subdirectory(common)
add_subdirectory(io)

find_package(Boost REQUIRED)

find_package(GLFW REQUIRED)
add_subdirectory(gfx)

# find_package(PulseAudio_Simple REQUIRED)
# find_package(FLAC REQUIRED)
# add_subdirectory(snd)

add_subdirectory(fps)

#add_subdirectory(assets)

find_package(Freetype REQUIRED)
add_subdirectory(ui)

add_subdirectory(strat)
add_subdirectory(gen)
add_subdirectory(terrain)
add_subdirectory(water)

add_subdirectory(collisionlib)

find_package(Threads REQUIRED)

add_executable(strategy main.cpp)
target_link_libraries(strategy gfxlib gfxsupportlib commonlib uilib stratlib
                               genlib terrainlib)
target_link_libraries(strategy ${CMAKE_THREAD_LIBS_INIT})


add_executable(fps_exe main_fps.cpp)
target_link_libraries(fps_exe gfxlib gfxsupportlib commonlib fpslib
                              collisionlib)

add_executable(sail main_sail.cpp)
target_link_libraries(sail gfxlib gfxsupportlib commonlib fpslib collisionlib
                           waterlib terrainlib)

# add_executable(play_snd play_snd.cpp)
# target_link_libraries(play_snd sndlib commonlib)

find_package(LuaJit REQUIRED)
add_subdirectory(luaint)

add_executable(map_gen map_gen.cpp)
target_link_libraries(map_gen genlib luaintlib)

add_executable(event_spread_test event_spread.cpp)
target_link_libraries(event_spread_test stratlib commonlib)

add_subdirectory(tests)
