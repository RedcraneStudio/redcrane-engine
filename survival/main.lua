
-- Copyright (c) 2015 Luke San Antonio
-- All rights reserved.

local rc = require("redcrane")
rc.engine = ...

rc:set_client(require("client"))
rc:set_server(require("server"))
