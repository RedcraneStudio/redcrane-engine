
# Copyright (C) 2014 Luke San Antonio
# All rights reserved.

find_path(Steamworks_INCLUDE_DIR steam_api.h HINTS ${Steamworks_ROOT}/public
          PATH_SUFFIXES steam)
find_library(Steamworks_LIB steam_api HINTS ${Steamworks_ROOT}
             PATH_SUFFIXES redistributable_bin/linux64)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Steamworks DEFAULT_MSG Steamworks_LIB
                                  Steamworks_INCLUDE_DIR)
