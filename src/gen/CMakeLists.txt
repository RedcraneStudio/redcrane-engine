
# Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
# All rights reserved.

add_library(genlib terrain.cpp radial_algorithm.cpp lake_radial.cpp
                   noise_algorithm.cpp)

target_link_libraries(genlib opensimplex commonlib ${LIBPNG_LIBRARIES})
target_include_directories(genlib PUBLIC ${LIBPNG_INCLUDE_DIRS})
