
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_library(taskslib render.cpp server.cpp)
target_include_directories(taskslib PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(taskslib PUBLIC engine_gl ${SDL2_LIBRARIES} iolib)
