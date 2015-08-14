
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_data(assets ALL mat/plane.json mat/house.json obj/plane.obj obj/yurt.obj
                    obj/govnm_building.obj obj/fps_terrain.obj
                    obj/vertical_log.obj
                    scene/default.json structure/structures.json
                    shader/basic/f shader/basic/v
                    shader/hud/f shader/hud/v
                    tex/cracked_soil.png tex/grass.png tex/topdown_terrain.png
                    tex/bark.png
                    ui/hud.json obj/terrain.obj mat/terrain.json
                    map/default.png scene/fps.json tex/yurt_skin.png
                    tex/combined_building_textures.png
                    shader/debug/v shader/debug/f)

include(convert_files)

convert_to_png(assets ui build_icon.svg build_icon.png)
