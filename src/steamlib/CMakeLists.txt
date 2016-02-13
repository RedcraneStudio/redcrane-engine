
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

# Wraps steamworks SDK for stuff like achievements so we don't have to
add_library(steamlib steamwrap.cpp)

target_link_libraries(steamlib PUBLIC ${Steamworks_LIB})
target_include_directories(steamlib PUBLIC ${Steamworks_INCLUDE_DIR})
