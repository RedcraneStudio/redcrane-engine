
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(sndlib STATIC common.cpp)
target_include_directories(sndlib PUBLIC ${PortAudio_INCLUDE_DIR})
target_link_libraries(sndlib ${PortAudio_LIBRARY})
