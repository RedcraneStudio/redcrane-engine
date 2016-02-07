
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(saillib game.cpp)

target_link_libraries(saillib PUBLIC uselib netlib taskslib boatlib)

# target_include_directories(saillib PUBLIC ${ENet_INCLUDE_DIR})
# target_link_libraries(saillib PUBLIC ${ENet_LIBRARY})
