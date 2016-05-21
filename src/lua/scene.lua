
-- Copyright (c) 2016 Luke San Antonio
-- All rights reserved.

local ffi = require("ffi")

local rc = require("redcrane")

local scene = {}

function scene.make_scene()

    local eng = rc.engine

    local sc = {}
    sc._scene_ptr = ffi.gc(ffi.C.redc_make_scene(eng), ffi.C.redc_unmake_scene)

    function sc:add_camera(tp)
        -- No need to use ffi.gc since cameras are going to automatically be
        -- deallocated with the engine
        return ffi.C.redc_scene_add_camera(self._scene_ptr, tp)
    end
    function sc:add_mesh(msh)
        return ffi.C.redc_scene_add_mesh(self._scene_ptr, msh)
    end

    function sc:add_player()
        -- Load gun for player hud and camera for a player.

        -- Request a new player (TODO Should fail when we already have one,
        -- this function is mostly a suggestion to get things moving).
        rc.server:req_player()

        local hand_mesh = rc.mesh:load_mesh("character/hands")
        local player = {
            hands = self:add_mesh(hand_mesh);
            camera = self:add_camera("fps")
        }
        self:set_parent(player.hands, player.camera)

        -- Figure the camera will follow the player whoever that be.
        self:camera_set_follow_player(player.camera, true)

        return player
    end

    function sc:active_camera(cam)
        -- Have a camera?
        if cam == nil then
            -- Change the camera
            return ffi.C.redc_scene_get_active_camera(self._scene_ptr)
        else -- No camera?
            -- Get the camera
            return ffi.C.redc_scene_set_active_camera(self._scene_ptr, cam)
        end
    end

    function sc:camera_set_follow_player(cam, val)
        val = val or true
        ffi.C.redc_scene_camera_set_follow_player(self._scene_ptr, cam, val)
    end

    -- A transformation applied to a mesh only really makes sense in the context
    -- of a scene!
    function sc:set_parent(obj, parent)
        ffi.C.redc_scene_set_parent(self._scene_ptr, obj, parent)
    end

    function sc:object_set_texture(obj, texture)
       ffi.C.redc_scene_object_set_texture(self._scene_ptr_, obj, texture)
    end

    function sc:step()
        ffi.C.redc_scene_step(self._scene_ptr)
    end

    function sc:render()
        ffi.C.redc_scene_render(self._scene_ptr)
    end

    return sc
end

return scene
