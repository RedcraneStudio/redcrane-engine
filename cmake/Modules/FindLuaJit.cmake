
# Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
# All rights reserved.

find_library(LuaJit_LIBRARY luajit-5.1)
find_path(LuaJit_INCLUDE_DIR luajit-2.0/lua.h)

message("Found lua - ${LuaJit_LIBRARY}")

if(APPLE)
  set(LuaJit_COMPILE_OPTIONS "-pagezero_size 10000 -image_base 100000000")
else()
  set(LuaJit_COMPILE_OPTIONS "")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LuaJit DEFAULT_MSG LuaJit_LIBRARY
                                  LuaJit_INCLUDE_DIR)
