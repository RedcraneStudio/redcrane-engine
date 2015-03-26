
# Copyright (C) 2014 Luke San Antonio
# All rights reserved.

find_package(PkgConfig)
pkg_check_modules(PC_LIBUV QUIET libuv)

find_path(LIBUV_INCLUDE_DIR uv.h HINTS ${PC_LIBUV_INCLUDE_DIRS})
find_library(LIBUV_LIBRARY uv HINTS ${PC_LIBUV_LIBRARY_DIRS})

set(LIBUV_INCLUDE_DIRS ${LIBUV_INCLUDE_DIR})
set(LIBUV_LIBRARIES ${LIBUV_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibUV, DEFAULT_MSG
                                  LIBUV_INCLUDE_DIR LIBUV_LIBRARY)
