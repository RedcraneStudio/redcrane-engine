
# Copyright (C) 2014 Luke San Antonio
# All rights reserved.

add_library(uilib views/label.cpp views/sprite.cpp views/bar.cpp
                  layouts/linear_layout.cpp layouts/side_layout.cpp
                  layouts/grid_layout.cpp load.cpp View.cpp
                  triggers/Mouse_Click.cpp triggers/Mouse_Hover.cpp
                  controllers/input.cpp)

target_link_libraries(uilib ${SDL2_LIBRARIES})
target_link_libraries(uilib commonlib gamelib)
