
-- Copyright (c) 2015 Luke San Antonio
-- All rights reserved.

-- Make Lua aware of our engine functions

local ffi = require("ffi")

local io = require("io")

local c_header = io.open("../src/redcrane_decl.h", "r")
local c_decl = c_header:read("*all")
c_header:close()

ffi.cdef(c_decl)
