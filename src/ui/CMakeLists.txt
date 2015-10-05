
# Copyright (C) 2014 Luke San Antonio
# All rights reserved.

add_library(uilib elements/layouts/grid_layout.cpp
                  elements/layouts/linear_layout.cpp
                  elements/layouts/side_layout.cpp
                  elements/bar.cpp elements/label.cpp elements/sprite.cpp
                  element.cpp load.cpp freetype_renderer.cpp mouse_logic.cpp
                  element_iterator.cpp simple_controller.cpp pie_menu.cpp)

target_link_libraries(uilib commonlib)

# We need this for boost::optional.
target_include_directories(uilib PUBLIC ${Boost_INCLUDE_DIRS})

target_include_directories(uilib PUBLIC ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(uilib ${FREETYPE_LIBRARIES})
