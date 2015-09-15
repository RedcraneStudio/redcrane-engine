
# Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
# All rights reserved.

add_library(luaintlib STATIC common.cpp gen.cpp)

target_link_libraries(luaintlib PUBLIC genlib)

target_link_libraries(luaintlib PUBLIC ${LuaJit_LIBRARY})
target_include_directories(luaintlib PUBLIC ${LuaJit_INCLUDE_DIR})

target_compile_options(luaintlib INTERFACE ${LuaJit_COMPILE_OPTIONS})

target_link_libraries(luaintlib PUBLIC ${LIBUV_LIBRARIES})
target_include_directories(luaintlib PUBLIC ${LIBUV_INCLUDE_DIR})
