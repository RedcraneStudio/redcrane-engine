
# Copyright (C) 2014 Luke San Antonio
# All rights reserved.

add_library(uilib elements/layouts/grid_layout.cpp
                  elements/layouts/linear_layout.cpp
                  elements/layouts/side_layout.cpp
                  elements/bar.cpp elements/label.cpp elements/sprite.cpp
                  controller.cpp element.cpp load.cpp)

target_link_libraries(uilib commonlib)

target_include_directories(uilib PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(uilib ${FREETYPE_LIBRARIES})
