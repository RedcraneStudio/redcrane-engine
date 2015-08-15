
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(commonlib STATIC log.cpp translate.cpp json.cpp animation.cpp
                             noise.cpp aabb.cpp debugging.cpp)
target_link_libraries(commonlib ${LIBUV_LIBRARIES})
target_include_directories(commonlib PUBLIC ${LIBUV_INCLUDE_DIRS})
