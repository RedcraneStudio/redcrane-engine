
-- Copyright (C) 2016 Luke San Antonio
-- All rights reserved.

-- Initialize the engine given a user-supplied configuration table

-- This first part shows the options available through the config table.

-- Get our config table
local cfg = ...

local rc = require("redcrane")

local config_spec = {
    cwd = {
        desc = "Current working directory",
        expected_type = "string",
        default = ".",
    },
    mod_name = {
        desc = "Mod name",
        expected_type = "string",
        default = "Redcrane Engine Game",
    },
    default_port = {
        desc = "Default port",
        expected_type = "number",
        default = 28222,
    },
    client_entry = {
        desc = "Client entry file",
        expected_type = "string",
        default = "client.lua",
    },
    server_entry = {
        desc = "Server entry file",
        expected_type = "string",
        default = "server.lua",
    },
}

-- Verifies the type of a given value.
function checktype(var, expect, name)
    assert(type(var) == expect, name.." must be a "..expect)
end

-- Build one config for C interface / engine consumption and the other for
-- access in the mod.
local modcfg = {}
for key, option in pairs(config_spec) do
    -- Get the value or default
    local option_value = cfg[key] or option.default
    -- If there is no default, make sure the value is given.
    assert(option_value ~= nil, "Value for option '"..key.."' not given")

    -- Make sure it is the correct type
    checktype(option_value, option.expected_type, option.desc)

    -- Add sanitized and checked value/default to the main config.
    modcfg[key] = option_value
end

-- Build a read-only table for config access in the mod's lua code
rc.config = setmetatable({}, {
    __index = modcfg,
    __newindex = function(table, key, value)
        error("Config option '"..self.desc.. "' is read-only during runtime")
    end,
    __metatable = false,
})

rc:init(modcfg)
