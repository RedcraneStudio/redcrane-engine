
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(netlib protocol.cpp net_io.cpp)
target_include_directories(netlib PUBLIC ${ENet_INCLUDE_DIR})
target_link_libraries(netlib PUBLIC iolib ${ENet_LIBRARY} commonlib)
