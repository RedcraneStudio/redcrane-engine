
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_data(assets ALL mat/plane.json mat/house.json obj/plane.obj obj/house.obj
                    obj/sphere.obj
                    scene/default.json structure/house.json
                    shader/basic/f shader/basic/v
                    tex/cracked_soil.png tex/grass.png)

include(convert_files)

convert_to_png(assets ui build_icon.svg build_icon.png)
