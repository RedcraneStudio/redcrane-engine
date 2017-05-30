
-- Copyright (C) 2016 Luke San Antonio
-- All rights reserved.

local ffi = require("ffi")

local rc = require("redcrane")

local server = {}

function server:req_player()
    return ffi.C.redc_server_req_player(rc.engine)
end

function server:running()
    return ffi.C.redc_server_running(rc.engine) == 1
end

function server:start(max_clients)
    return ffi.C.redc_server_start(rc.engine, max_clients)
end

function server:step()
    ffi.C.redc_server_step(rc.engine)
end

return server
