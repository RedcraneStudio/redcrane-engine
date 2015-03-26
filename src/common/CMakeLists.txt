
# Copyright (C) 2014 Luke San Antonio
# All rights reserved.

add_library(commonlib STATIC log.cpp translate.cpp)
target_link_libraries(commonlib ${LIBUV_LIBRARIES})
