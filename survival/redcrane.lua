
-- Copyright (c) 2015 Luke San Antonio
-- All rights reserved.

local ffi = require("ffi")

local c_header = io.open("/home/luke/projects/redcrane/engine/src/redcrane_decl.h", "r")
local c_decl = c_header:read("*all")
c_header:close()
ffi.cdef(c_decl)

local rc = {}

-- rc.engine is expected to be set by the main script

function rc:default_scene()
    return ffi.C.redc_get_default_scene(self.engine)
end

return rc
