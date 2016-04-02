
-- Copyright (C) 2016 Luke San Antonio
-- All rights reserved.

-- Making the sandbox environment in Lua is so much easier because we have to
-- make sure we don't mutate the global environment and properly references
-- variables that *are* in the global environment.

local env = {}

env.require = require

env.package = {}
env.package.preload = {redcrane = package.preload.redcrane}
env.package.loaders = {package.loaders[1] }

return env
