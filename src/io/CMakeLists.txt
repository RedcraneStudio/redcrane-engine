
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(iolib STATIC external_io.cpp ipc.cpp net.cpp net_io.cpp buffer.cpp)
target_include_directories(iolib PUBLIC ${ENet_INCLUDE_DIR})
target_link_libraries(iolib commonlib ${LIBUV_LIBRARIES} ${ENet_LIBRARY})
