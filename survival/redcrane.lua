
-- Copyright (c) 2015 Luke San Antonio
-- All rights reserved.

local ffi = require("ffi")

local c_header = io.open("/home/luke/projects/redcrane/engine/src/redcrane_decl.h", "r")
local c_decl = c_header:read("*all")
c_header:close()

ffi.cdef(c_decl)

local rc = {}

function rc.get_config_opt(eng, opt)
    local str = ffi.C.redc_get_config_opt(eng, opt)
    return ffi.gc(str, ffi.C.free);
end

return rc
