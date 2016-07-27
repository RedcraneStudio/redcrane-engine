
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(commonlib STATIC aabb.cpp animation.cpp json.cpp
                             log.cpp noise.cpp translate.cpp tree.cpp task.cpp)
target_link_libraries(commonlib PUBLIC ${LIBUV_LIBRARIES} opensimplex)
target_include_directories(commonlib PUBLIC ${LIBUV_INCLUDE_DIRS}
                                            ${GLM_INCLUDE_DIR}
                                            ${BULLET_INCLUDE_DIRS}
                                            ${Msgpack_INCLUDE_DIR})
