
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

include_directories(${CMAKE_SOURCE_DIR})

find_package(Threads REQUIRED)

find_package(Msgpack REQUIRED)
include_directories(${Msgpack_INCLUDE_DIR})

find_package(LibPNG REQUIRED)
find_package(LibUV REQUIRED)
find_package(GLM REQUIRED)
add_subdirectory(common)
add_subdirectory(io)

find_package(ENet REQUIRED)
add_subdirectory(net)

find_package(Boost REQUIRED COMPONENTS system program_options filesystem)

find_package(SDL2 REQUIRED)
add_subdirectory(gfx)
add_subdirectory(input)

find_package(LuaJit REQUIRED)
find_package(Freetype REQUIRED)

add_subdirectory(fps)
add_subdirectory(use)
add_subdirectory(assets)

find_package(Bullet REQUIRED)

add_executable(redc main.cpp minilua.cpp redcrane.cpp sdl_helper.cpp player.cpp)

target_include_directories(redc PUBLIC ${SDL2_INCLUDE_DIRS}
                                       ${BULLET_INCLUDE_DIRS})

target_link_libraries(redc PUBLIC commonlib netlib gfxlib uselib fpslib
                                  inputlib
                                  ${Boost_SYSTEM_LIBRARY}
                                  ${Boost_PROGRAM_OPTIONS_LIBRARY}
                                  ${Boost_FILESYSTEM_LIBRARY}
                                  ${SDL2_LIBRARIES}
                                  ${BULLET_LIBRARIES})

target_link_libraries(redc PUBLIC ${LuaJit_LIBRARY})
target_include_directories(redc PUBLIC ${LuaJit_INCLUDE_DIR})
target_compile_options(redc PUBLIC ${LuaJit_COMPILE_OPTIONS})

add_dependencies(redc survival)
add_dependencies(redc assets)
