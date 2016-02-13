
# Copyright (C) 2014 Luke San Antonio
# All rights reserved.

set(SUBDIR "")
if(UNIX)
    if(NOT APPLE)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(SUBDIR "linux64")
        else()
            set(SUBDIR "linux32")
        endif()
    # We are on OS X
    else()
        set(SUBDIR "osx32")
    endif()

elseif(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SUBDIR "win64")
    else()
        message(SEND_ERROR "Cannot build on 32bit because of Steamworks")
    endif()

    set(SUBDIR "win64")
endif()


find_path(Steamworks_INCLUDE_DIR steam/steam_api.h HINTS
          ${Steamworks_ROOT}/public)
find_library(Steamworks_LIB steam_api HINTS ${Steamworks_ROOT}
             PATH_SUFFIXES redistributable_bin/${SUBDIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Steamworks DEFAULT_MSG Steamworks_LIB
                                  Steamworks_INCLUDE_DIR)
