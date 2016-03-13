
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(inputlib input.cpp)
target_include_directories(inputlib PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(inputlib PUBLIC commonlib ${SDL2_LIBRARIES})
