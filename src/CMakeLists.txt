
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

include_directories(${CMAKE_SOURCE_DIR})

find_package(LibPNG REQUIRED)
find_package(LibUV REQUIRED)
add_subdirectory(common)
add_subdirectory(io)

find_package(GLFW REQUIRED)
add_subdirectory(gfx)

find_package(PulseAudio_Simple REQUIRED)
find_package(FLAC REQUIRED)
add_subdirectory(snd)

add_subdirectory(fps)

#find_package(Boost REQUIRED COMPONENTS filesystem)
#add_subdirectory(assets)

find_package(Freetype REQUIRED)
add_subdirectory(ui)

add_subdirectory(stratlib)

add_subdirectory(map)

find_package(Threads REQUIRED)

add_executable(strategy main.cpp)
target_link_libraries(strategy gfxlib gfxsupportlib commonlib uilib maplib
                               sndlib)
target_link_libraries(strategy ${CMAKE_THREAD_LIBS_INIT})


find_package(Bullet REQUIRED)

add_executable(fps_exe main_fps.cpp)
target_link_libraries(fps_exe gfxlib gfxsupportlib commonlib fpslib)
target_link_libraries(fps_exe ${BULLET_LIBRARIES})

target_include_directories(fps_exe PUBLIC ${BULLET_INCLUDE_DIRS})

add_executable(play_snd play_snd.cpp)
target_link_libraries(play_snd sndlib commonlib)

add_subdirectory(tests)
