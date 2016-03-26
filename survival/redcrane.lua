
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

function rc:running()
    ffi.C.redc_running(rc.engine)
end
function rc:step()
    ffi.C.redc_step(rc.engine)
end

return rc
