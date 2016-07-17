
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(sndlib STATIC common.cpp flac.cpp)

target_link_libraries(sndlib commonlib)

target_include_directories(sndlib PUBLIC ${SDL2_INCLUDE_DIR})
target_link_libraries(sndlib ${SDL2_LIBRARIES})

target_link_libraries(sndlib ${FLAC_LIBRARY})
