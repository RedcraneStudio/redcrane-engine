
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

  -- Initialize the engine client
  rc:init_client()
  rc:init_server()

  -- Pass control to the user client script
  local client, err = loadfile(rc.config.client_entry)
  if client == nil then
      error(err)
  end
  setfenv(client, sandbox)
  return client()
else
  rc:log_i("Invalid server mode '"..server_mode.."'")
  return 1
end

return 0
