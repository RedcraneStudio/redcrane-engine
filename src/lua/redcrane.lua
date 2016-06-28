
-- Copyright (c) 2015 Luke San Antonio
-- All rights reserved.

local ffi = require("ffi")

local rc = {}

function rc:init(config)
    self.engine = self.engine or ffi.gc(ffi.C.redc_init_engine(config),
                                        ffi.C.redc_uninit_engine)

    self.scene = require("scene")
    self.mesh = require("mesh")
    self.texture = require("texture")
    self.shader = require("shader")
    self.server = require("server")

    return self.engine
end

function rc:init_client()
    ffi.C.redc_init_client(self.engine)
end
function rc:init_server()
    ffi.C.redc_init_server(self.engine)
end

function rc:step()
    return ffi.C.redc_step_engine(self.engine)
end

function rc:render()
    return ffi.C.redc_render_engine(self.engine)
end

function rc:running()
    return ffi.C.redc_running(self.engine)
end
function rc:swap_window()
    ffi.C.redc_window_swap(self.engine)
end

function rc:asset_path()
    -- This memory doesn't have to be freed
    return ffi.C.redc_get_asset_path(self.engine)
end

function rc:load_map(filename)
    return ffi.C.redc_map_load(self.engine, filename)
end
function rc:load_hud(filename)
    return ffi.C.redc_hud_load(self.engine, filename)
end
function rc:load_skin(filename)
    return ffi.C.redc_skin_load(self.engine, filename)
end

function rc:load_game_set(set)
    if set.map then
        rc:load_map(set.map)
    end
    if set.hud then
        rc:load_hud(set.hud)
    end
    if set.skin then
        rc:load_skin(set.skin)
    end
end

-- Logging functions
function stringify(str, ...)
    for n=1,select('#', ...) do
        local e = select(n, ...)
        str = str .. ' ' .. tostring(e)
    end
    return str
end

rc.internal = {}

function rc.internal:log_d(str, ...)
    ffi.C.redc_log_d(stringify(tostring(str), ...))
end
function rc.internal:log_i(str, ...)
    ffi.C.redc_log_i(stringify(tostring(str), ...))
end
function rc.internal:log_w(str, ...)
    ffi.C.redc_log_w(stringify(tostring(str), ...))
end
function rc.internal:log_e(str, ...)
    ffi.C.redc_log_e(stringify(tostring(str), ...))
end

local prefix = "(Mod) "
function rc:log_d(str, ...)
    rc.internal:log_d(prefix, str, ...)
end
function rc:log_i(str, ...)
    rc.internal:log_i(prefix, str, ...)
end
function rc:log_w(str, ...)
    rc.internal:log_w(prefix, str, ...)
end
function rc:log_e(str, ...)
    rc.internal:log_e(prefix, str, ...)
end

return rc
