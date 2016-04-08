
-- Copyright (c) 2015 Luke San Antonio
-- All rights reserved.

local ffi = require("ffi")

local rc = {}

function rc:init(config)
    self.engine = self.engine or ffi.gc(ffi.C.redc_init_engine(config),
                                        ffi.C.redc_uninit_engine)

    self.scene = require("scene")
    self.mesh_pool = require("mesh_pool")
    self.shader = require("shader")

    return self.engine
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

-- Logging functions
function stringify(str, ...)
    for n=1,select('#', ...) do
        local e = select(n, ...)
        str = str .. ' ' .. tostring(e)
    end
    return str
end
function rc:log_d(str, ...)
    ffi.C.redc_log_d(stringify(str, ...))
end
function rc:log_i(str, ...)
    ffi.C.redc_log_i(stringify(str, ...))
end
function rc:log_w(str, ...)
    ffi.C.redc_log_w(stringify(str, ...))
end
function rc:log_e(str, ...)
    ffi.C.redc_log_e(stringify(str, ...))
end

return rc
