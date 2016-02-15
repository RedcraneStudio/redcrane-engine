
# Copyright (C) 2016 Luke San Antonio
# All rights reserved.

add_library(assetslib load.cpp)
target_link_libraries(assetslib ${Boost_SYSTEM_LIBRARY}
                                ${Boost_FILESYSTEM_LIBRARY})
