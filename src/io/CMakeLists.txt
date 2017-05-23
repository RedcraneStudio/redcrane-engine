
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(iolib STATIC external_io.cpp ipc.cpp buffer.cpp)
target_link_libraries(iolib commonlib ${LIBUV_LIBRARIES})
