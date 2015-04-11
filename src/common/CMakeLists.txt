
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(commonlib STATIC log.cpp translate.cpp json.cpp mesh.cpp)
target_link_libraries(commonlib ${LIBUV_LIBRARIES})
