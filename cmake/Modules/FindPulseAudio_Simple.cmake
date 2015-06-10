
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

find_library(PulseAudio_Simple_LIBRARY pulse-simple)
find_library(PulseAudio_LIBRARY pulse)
find_path(PulseAudio_Simple_INCLUDE_DIR simple.h PATH_SUFFIXES pulse)

set(PulseAudio_Simple_LIBRARIES "${PulseAudio_Simple_LIBRARY};\
${PulseAudio_LIBRARY}" CACHE INTERNAL "")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PulseAudio_Simple DEFAULT_MSG
                                  PulseAudio_Simple_LIBRARY
                                  PulseAudio_Simple_INCLUDE_DIR)
