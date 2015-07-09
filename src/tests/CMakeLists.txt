
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_executable(engine_test_suite maybe_owned.cpp mesh.cpp grid_iterator.cpp
                                 software_texture.cpp element_iterator.cpp
                                 triangle_conversion.cpp circle.cpp)
target_link_libraries(engine_test_suite commonlib gfxsupportlib gfxlib uilib
                                        collisionlib)
