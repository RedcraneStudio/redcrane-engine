
# Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
# All rights reserved.

add_library(procedurallib terrain.cpp)

target_link_libraries(procedurallib opensimplex commonlib ${LIBPNG_LIBRARIES})
target_include_directories(gfxsupportlib PUBLIC ${LIBPNG_INCLUDE_DIRS})
