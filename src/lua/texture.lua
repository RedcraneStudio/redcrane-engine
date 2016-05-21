
-- Copyright (c) 2016 Luke San Antonio
-- All rights reserved.

local ffi = require("ffi")

local rc = require("redcrane")

local texture = {}

function texture:load_texture(str)
   return ffi.gc(ffi.redc_load_texture(rc.engine, str),
                 ffi.C.redc_unload_texture)
end

return texture
