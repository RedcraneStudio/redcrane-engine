
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_executable(engine_test_suite maybe_owned.cpp grid_iterator.cpp
                                 software_texture.cpp element_iterator.cpp)
target_link_libraries(engine_test_suite commonlib gfxsupportlib gfxlib uilib)
