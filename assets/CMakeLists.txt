
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_data(assets ALL mat/plane.json mat/house.json obj/plane.obj obj/house.obj
                    obj/sphere.obj obj/fps_terrain.obj
                    scene/default.json structure/structures.json
                    shader/basic/f shader/basic/v
                    shader/hud/f shader/hud/v
                    tex/cracked_soil.png tex/grass.png tex/topdown_terrain.png
                    ui/hud.json obj/terrain.obj mat/terrain.json
                    map/default.png scene/fps.json)

include(convert_files)

convert_to_png(assets ui build_icon.svg build_icon.png)
