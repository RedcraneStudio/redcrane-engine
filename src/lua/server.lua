
-- Copyright (C) 2016 Luke San Antonio
-- All rights reserved.

local ffi = require("ffi")

local rc = require("redcrane")

local server = {}

function server:req_player()
    return ffi.C.redc_server_req_player(rc.engine)
end

return server
