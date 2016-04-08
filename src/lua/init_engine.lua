
-- Copyright (C) 2016 Luke San Antonio
-- All rights reserved.

-- Initialize the engine given a user-supplied configuration table

-- This first part shows the options available through the config table.

-- Get our config table
local cfg = ...

local rc = require("redcrane")

local options = {
    cwd = {
        desc = "Current working directory",
        expected_type = "string",
        default = ".",
        runtime = false,
    },
    mod_name = {
        desc = "Mod name",
        expected_type = "string",
        default = "Redcrane Engine Game",
        runtime = false,
    },
    default_port = {
        desc = "Default port",
        expected_type = "number",
        default = 28222,
        runtime = false,
    },
    client_entry = {
        desc = "Client entry file",
        expected_type = "string",
        default = "client.lua",
        runtime = false,
    },
    server_entry = {
        desc = "Server entry file",
        expected_type = "string",
        default = "server.lua",
        runtime = false,
    },
}

function checktype(var, expect, name)
    assert(type(var) == expect, name.." must be a "..expect)
end

-- Put each option in a new config
local newcfg = {}
local engcfg = {}
for key, option in pairs(options) do

    -- Use default if the user didn't provide one
    local option_value = cfg[key] or option.default

    -- Make sure it isn't nil
    assert(option_value ~= nil, "Value for option '"..key.."' not given")

    -- Make sure it is the correct type
    checktype(option_value, option.expected_type, option.desc)

    local get_fn
    local set_fn
    if option.runtime then
        -- Use the provided get function
        get_fn = function(self)
            return option.get_fn()
        end

        -- Verify type and use setter
        set_fn = function(self, value)
            checktype(value, option.expected_type, option.desc)
            -- Use a closure here so we don't have to expose the C function
            -- directly, they have to go through us and the type checking.
            return option.set_fn(value)
        end
    else
        -- Just return the provided value
        get_fn = function(self)
            return self.value
        end

        -- Fail
        set_fn = function(self, value)
            error("Config option '"..self.name.. "' read-only during runtime")
        end
    end

    -- Put it in the new config
    newcfg[key] = {
        name = option.name,
        value = option_value,
        get = get_fn,
        set = set_fn
    }
    -- Use this config to initialize the engine
    engcfg[key] = option_value
end

newcfg = setmetatable(newcfg, {
    __index = function(table, key)
        -- Call the individual get function for this particular option with
        -- itself as the parameter.
        return rawget(table, key):get()
    end,
    __newindex = function(table, key, value)
        local option = rawget(table, key)
        option.value = option:set(value)
    end
})

rc:init(engcfg)
rc.config = newcfg
