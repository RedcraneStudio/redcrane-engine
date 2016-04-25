
-- Copyright (C) 2016 Luke San Antonio
-- All rights reserved.

local ffi = require("ffi")

local rc = require("redcrane")

local server = {}

function server:make_player()
    return ffi.C.redc_server_make_player(rc.engine)
end

return server
