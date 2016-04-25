
-- Copyright (c) 2016 Luke San Antonio
-- All rights reserved.

local ffi = require("ffi")

local rc = require("redcrane")

local mesh = {}

function mesh:load_mesh(str)
    return ffi.gc(ffi.C.redc_load_mesh(rc.engine, str), ffi.C.redc_unload_mesh)
end

return mesh
