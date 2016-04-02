
-- Copyright (c) 2015 Luke San Antonio
-- All rights reserved.

local ffi = require("ffi")

local io = require("io")

local c_header = io.open("../src/redcrane_decl.h", "r")
local c_decl = c_header:read("*all")
c_header:close()

ffi.cdef(c_decl)

local rc = {}

-- TODO: Potentially clean up the config
local config = require("config")

rc.engine = ffi.gc(ffi.C.redc_init_engine(config), ffi.C.redc_uninit_engine)

rc.scene = require("scene")
rc.mesh_pool = require("mesh_pool")
rc.shader = require("shader")

function rc:running()
    return ffi.C.redc_running(rc.engine)
end
function rc:swap_window()
    ffi.C.redc_window_swap(rc.engine)
end

function rc:asset_path()
    -- This memory doesn't have to be freed
    return ffi.C.redc_get_asset_path(rc.engine)
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
