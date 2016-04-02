
# Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
# All rights reserved.

find_library(LuaJIT_LIBRARY luajit-5.1)
find_path(LuaJIT_INCLUDE_DIR luajit.h PATH_SUFFIXES luajit-2.0)

if(APPLE)
  set(LuaJIT_COMPILE_OPTIONS "-pagezero_size 10000 -image_base 100000000")
else()
  set(LuaJIT_COMPILE_OPTIONS "")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LuaJIT_DEFAULT_MSG LuaJIT_LIBRARY
                                  LuaJIT_INCLUDE_DIR LuaJIT_BIN)
