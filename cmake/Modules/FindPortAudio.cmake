
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

find_library(PortAudio_LIBRARY portaudio)
find_path(PortAudio_INCLUDE_DIR portaudio.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PortAudio DEFAULT_MSG PortAudio_LIBRARY
                                  PortAudio_INCLUDE_DIR)
