
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_data(assets ALL mat/plane.json mat/house.json obj/plane.obj obj/yurt.obj
                    obj/govnm_building.obj obj/fps_terrain.obj
                    obj/vertical_log.obj obj/map.obj obj/boat.obj
                    obj/projectile.obj

                    obj/guns/cannon.obj
                    obj/guns/gattlinggun.obj
                    obj/hulls/sailhull.obj
                    obj/rudders/rudder.obj
                    obj/sails/sail.obj

                    scene/default.json structure/structures.json
                    shader/basic/fs.glsl shader/basic/vs.glsl
                    shader/hud/fs.glsl shader/hud/vs.glsl
                    shader/debug/vs.glsl shader/debug/fs.glsl
                    shader/terrain/vs.glsl shader/terrain/fs.glsl
                    shader/water/vs.glsl shader/water/fs.glsl
                    shader/envmap/vs.glsl shader/envmap/fs.glsl

                    tex/envmap/down.png
                    tex/envmap/up.png
                    tex/envmap/back.png
                    tex/envmap/front.png
                    tex/envmap/left.png
                    tex/envmap/right.png

                    cfg.ini

                    tex/cracked_soil.png tex/grass.png tex/topdown_terrain.png
                    tex/bark.png
                    ui/hud.json obj/terrain.obj mat/terrain.json
                    map/default.png scene/fps.json tex/yurt_skin.png
                    tex/combined_building_textures.png)

include(convert_files)

convert_to_png(assets ui build_icon.svg build_icon.png)
