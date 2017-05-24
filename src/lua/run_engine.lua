-- Copyright (C) 2016 Luke San Antonio
-- All rights reserved.

local rc = require("redcrane")

local server_mode, sandbox = ...

if server_mode == "dedicated" then
    rc:log_i("Starting dedicated server")
elseif server_mode == "connect" then
    rc:log_i("Connecting")
elseif server_mode == "local" then
    rc:log_i("Starting local server")

    -- Load client
    local client, err = loadfile(rc.config.client_entry)
    if client == nil then
        error(err)
    end
    setfenv(client, sandbox)

    -- Make a coroutine for the server and client to interact
    local co = coroutine.create(client)

    -- Load server script
    local server, err = loadfile(rc.config.server_entry)
    if server == nil then
        error(err)
    end
    setfenv(server, sandbox)

    -- Initialize the engine client and server side
    rc:init_client()
    rc:init_server()

    return server(co)
else
    rc:log_i("Invalid server mode '" .. server_mode .. "'")
    return 1
end

return 0
