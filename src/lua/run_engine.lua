-- Copyright (C) 2016 Luke San Antonio
-- All rights reserved.

local rc = require("redcrane")

local server_mode, sandbox = ...

function load_script(filename)
    -- Load script
    local script, err = loadfile(filename)
    if script == nil then
        error(err)
    end
    -- Apply sandbox
    setfenv(script, sandbox)
    return script
end

if server_mode == "dedicated" then
    rc:log_i("Starting dedicated server")

    local server = load_script(rc.config.server_entry)
    rc:init_server()
    return server()

elseif server_mode == "connect" then
    rc:log_i("Connecting")

    local client = load_script(rc.config.client_entry)
    rc:init_client()
    return client()
elseif server_mode == "local" then
    rc:log_i("Starting local server")

    local client = load_script(rc.config.client_entry)

    -- Make a coroutine for the server and client to interact
    local co = coroutine.create(client)

    local server = load_script(rc.config.server_entry)

    -- Initialize the engine client and server side
    rc:init_client()
    rc:init_server()

    return server(co)
else
    rc:log_i("Invalid server mode '" .. server_mode .. "'")
    return 1
end

return 0
