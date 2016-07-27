
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(collisionlib STATIC triangle_conversion.cpp triangle.cpp
                                sweep_and_prune.cpp motion.cpp)
target_include_directories(collisionlib PUBLIC ${GLM_INCLUDE_DIR}
                                               ${Boost_INCLUDE_DIRS})
