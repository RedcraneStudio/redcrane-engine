
# Copyright (C) 2016 Luke San Antonio
# All rights reserved.

add_library(assetslib fs_cache.cpp load_dir.cpp minigltf.cpp)
target_link_libraries(assetslib ${Boost_SYSTEM_LIBRARY}
                                ${Boost_FILESYSTEM_LIBRARY})
