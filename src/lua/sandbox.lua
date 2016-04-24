
-- Copyright (C) 2016 Luke San Antonio
-- All rights reserved.

-- Add redcrane stuff to the configuration sandbox
local rc = require("redcrane")

function deepcopy(orig)
    local orig_type = type(orig)
    local copy
    if orig_type == 'table' then
        copy = {}
        for orig_key, orig_value in next, orig, nil do
            copy[deepcopy(orig_key)] = deepcopy(orig_value)
        end
        setmetatable(copy, deepcopy(getmetatable(orig)))
    else -- number, string, boolean, etc
        copy = orig
    end
    return copy
end

local include_redcrane = ...

return function()
    local sandbox = require("config_sandbox")()

    if include_redcrane == true then

        sandbox.redcrane = {
            engine = rc.engine,
            config = rc.config,

            running = rc.running,
            swap_window = rc.swap_window,
            asset_path = rc.asset_path,

            step_server = rc.step_server,

            load_map = rc.load_map,

            scene = {
                make_scene = rc.scene.make_scene
            },
            mesh_pool = {
                make_mesh_pool = rc.mesh_pool.make_mesh_pool
            },
            shader = {
                load_shader = rc.shader.load_shader
            },

            log_d = rc.log_d,
            log_i = rc.log_i,
            log_w = rc.log_w,
            log_e = rc.log_e
        }
    end

    return sandbox
end
