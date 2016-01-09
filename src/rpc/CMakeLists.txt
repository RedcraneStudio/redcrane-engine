
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(rpclib STATIC msgpack_interface.cpp req.cpp dispatch.cpp)

target_link_libraries(rpclib commonlib iolib)
