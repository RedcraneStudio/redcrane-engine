
# Copyright (C) 2014 Luke San Antonio
# All rights reserved.

# This is basically the FindLibUV.cmake module except with pngs.

find_package(PkgConfig)
pkg_check_modules(PC_LIBPNG QUIET libpng)

find_path(LIBPNG_INCLUDE_DIR png.h HINTS ${PC_LIBPNG_INCLUDE_DIRS})
find_library(LIBPNG_LIBRARY png HINTS ${PC_LIBPNG_LIBRARY_DIRS})

set(LIBPNG_INCLUDE_DIRS ${LIBPNG_INCLUDE_DIR})
set(LIBPNG_LIBRARIES ${LIBPNG_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibPNG, DEFAULT_MSG
                                  LIBPNG_INCLUDE_DIR LIBPNG_LIBRARY)

