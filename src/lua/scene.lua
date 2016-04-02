
-- Copyright (c) 2016 Luke San Antonio
-- All rights reserved.

local ffi = require("ffi")

local scene = {}

function scene.make_scene(eng)
    local sc = {}
    sc._scene_ptr = ffi.gc(ffi.C.redc_make_scene(eng), ffi.C.redc_unmake_scene)

    function sc:add_camera(tp)
        -- No need to use ffi.gc since cameras are going to automatically be
        -- deallocated with the engine
        return ffi.C.redc_scene_add_camera(self._scene_ptr, tp)
    end

    function sc:activate_camera(cam)
        return ffi.C.redc_scene_activate_camera(self._scene_ptr, cam)
    end
    function sc:active_camera()
        return ffi.C.redc_scene_get_active_camera(self._scene_ptr)
    end

    -- A transformation applied to a mesh only really makes sense in the context
    -- of a scene!
    function sc:attach(mesh, parent)
        if parent == nil then
            ffi.C.redc_scene_attach(self._scene_ptr, mesh, 0)
        else
            ffi.C.redc_scene_attach(self._scene_ptr, mesh, parent)
        end
    end

    function sc:render()
        ffi.C.redc_scene_render(self._scene_ptr)
    end

    function sc:step()
        ffi.C.redc_scene_step(self._scene_ptr)
    end

    return sc
end

return scene
