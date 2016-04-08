
-- Copyright (C) 2016 Luke San Antonio
-- All rights reserved.

local rc = require("redcrane")

local eng, server_mode, sandbox = ...

if server_mode == "dedicated" then
  rc:log_i("Starting dedicated server")
elseif server_mode == "connect" then
  rc:log_i("Connecting")
elseif server_mode == "local" then
  rc:log_i("Starting local server")
else
  rc:log_i("Invalid server mode '"..server_mode.."'")
  return 1
end

return 0
