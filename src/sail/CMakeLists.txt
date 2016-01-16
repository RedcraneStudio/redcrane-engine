
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(saillib player_data.cpp boat.cpp boat_def.cpp game.cpp methods.cpp)

target_link_libraries(saillib PUBLIC uselib iolib rpclib taskslib)

# target_include_directories(saillib PUBLIC ${ENet_INCLUDE_DIR})
# target_link_libraries(saillib PUBLIC ${ENet_LIBRARY})
