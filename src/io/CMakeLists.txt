
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(iolib STATIC external_io.cpp ipc.cpp net.cpp)
target_link_libraries(iolib commonlib)
