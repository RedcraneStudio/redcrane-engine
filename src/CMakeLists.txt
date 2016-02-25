
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
find_package(GLFW REQUIRED)
add_subdirectory(gfx)

find_package(LuaJit REQUIRED)
find_package(Freetype REQUIRED)

find_package(Steamworks REQUIRED)

# find_package(PulseAudio_Simple REQUIRED)
# find_package(FLAC REQUIRED)
# add_subdirectory(snd)

add_subdirectory(fps)

#add_subdirectory(assets)

# add_subdirectory(ui)

# add_subdirectory(strat)
add_subdirectory(gen)
add_subdirectory(terrain)
add_subdirectory(water)

add_subdirectory(collisionlib)

add_subdirectory(use)

add_subdirectory(boatlib)
add_subdirectory(effects)
add_subdirectory(tasks)


add_subdirectory(sail)

add_subdirectory(steamlib)

add_subdirectory(assets)

add_executable(redc main.cpp minilua.cpp redcrane.cpp)
target_link_libraries(redc PUBLIC commonlib ${Boost_SYSTEM_LIBRARY}
                                  ${Boost_PROGRAM_OPTIONS_LIBRARY}
                                  ${Boost_FILESYSTEM_LIBRARY}
                                  netlib)

target_link_libraries(redc PUBLIC ${LuaJit_LIBRARY})
target_include_directories(redc PUBLIC ${LuaJit_INCLUDE_DIR})
target_compile_options(redc PUBLIC ${LuaJit_COMPILE_OPTIONS})

add_dependencies(redc survival)

# We don't need these rpath settings on windows, the only thing we are
# installing is libsteam_api which will be installed in the binary directory on
# windows anyway.
#if(UNIX)
    #if(APPLE)
        #set_target_properties(sl PROPERTIES INSTALL_RPATH
                                 #"@executable_path/../lib")
    #else()
        #set_target_properties(sl PROPERTIES INSTALL_RPATH "\$ORIGIN/../lib")
    #endif()
#endif()

#install(TARGETS sl RUNTIME DESTINATION bin)

# Install the steamworks redistributable in bin/ if we are on windows and
# into lib/ otherwise
if(WIN32)
    install(FILES ${Steamworks_LIB} DESTINATION bin)
else()
    install(FILES ${Steamworks_LIB} DESTINATION lib)
endif()

# add_executable(play_snd play_snd.cpp)
# target_link_libraries(play_snd sndlib commonlib)

#add_subdirectory(luaint)

#add_executable(map_gen map_gen.cpp)
#target_link_libraries(map_gen genlib luaintlib)

# add_executable(event_spread_test event_spread.cpp)
# target_link_libraries(event_spread_test stratlib commonlib)
