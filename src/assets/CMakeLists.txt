
# Copyright (C) 2014 Luke San Antonio
# All rights reserved.

add_library(assetslib discovery.cpp load.cpp)

target_include_directories(assetslib PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(assetslib ${Boost_filesystem_LIBRARY})
