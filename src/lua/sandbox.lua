
-- Copyright (C) 2016 Luke San Antonio
-- All rights reserved.

-- Add redcrane stuff to the configuration sandbox
local rc = require("redcrane")

function deepcopy(orig)
    local orig_type = type(orig)
    local copy
    if orig_type == 'table' then
        copy = {}
        for orig_key, orig_value in next, orig, nil do
            copy[deepcopy(orig_key)] = deepcopy(orig_value)
        end
        setmetatable(copy, deepcopy(getmetatable(orig)))
    else -- number, string, boolean, etc
        copy = orig
    end
    return copy
end

local include_redcrane = ...

return function()
    local sandbox = require("config_sandbox")()

    if include_redcrane == true then
        sandbox.redcrane = rc
    end

    return sandbox
end
