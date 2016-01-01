
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(commonlib STATIC aabb.cpp animation.cpp debugging.cpp json.cpp
                             log.cpp noise.cpp translate.cpp tree.cpp)
target_link_libraries(commonlib ${LIBUV_LIBRARIES})
target_include_directories(commonlib PUBLIC ${LIBUV_INCLUDE_DIRS})

target_include_directories(commonlib PUBLIC ${GLM_INCLUDE_DIR})
