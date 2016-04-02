
-- Copyright (c) 2016 Luke San Antonio
-- All rights reserved.

local ffi = require("ffi")

local shader = {}

function shader.load_shader(eng, dir)
    return ffi.gc(ffi.C.redc_make_shader(eng, dir), ffi.C.redc_unmake_shader)
end

return shader
