
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_data(assets ALL "share/${PROJECT_NAME}"

                    cfg.ini

                    snd/walk_variation_0.wav
                    snd/walk_variation_1.wav
                    snd/walk_variation_2.wav
                    snd/walk_variation_3.wav
                    snd/walk_variation_4.wav

                    gltf/cel.gltf gltf/cel_solid.vs gltf/cel_solid.fs
                    gltf/cel_texture.vs gltf/cel_texture.fs

                    gltf/deferred.gltf gltf/deferred.vs gltf/deferred.fs
                    gltf/library-pre.gltf

                    shader/deferred_fb_write.vs shader/deferred_fb_write.fs

                    shader/basic/fs.glsl shader/basic/vs.glsl
                    shader/hud/fs.glsl shader/hud/vs.glsl
                    shader/debug/vs.glsl shader/debug/fs.glsl
                    shader/terrain/vs.glsl shader/terrain/fs.glsl
                    shader/water/vs.glsl shader/water/fs.glsl
                    shader/envmap/vs.glsl shader/envmap/fs.glsl
                    shader/cel/vs.glsl shader/cel/fs.glsl
                    shader/text/vs.glsl shader/text/fs.glsl

                    shader/basic.lua shader/hud.lua

                    tex/envmap/down.png
                    tex/envmap/up.png
                    tex/envmap/back.png
                    tex/envmap/front.png
                    tex/envmap/left.png
                    tex/envmap/right.png

                    tex/crosshair.png)
