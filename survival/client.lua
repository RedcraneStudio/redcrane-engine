
-- Copyright (c) 2016 Luke San Antonio
-- All rights reserved.

local rc = require("redcrane")

local scene = require("scene")
local mesh_pool = require("mesh_pool")

local client = {}

function client:init(eng)
    -- What scene shall we use?
    self.scene = scene.make_scene(rc.engine)

    -- Use an fps camera, it's the only one so it will be made active.
    self.scene:add_camera("fps")

    -- Load in the character's hands
    self.mesh_pool = mesh_pool.make_mesh_pool(rc.engine)
    local hand_mesh = self.mesh_pool:load_mesh("character/hands")

    -- Attach the hands to the camera
    self.hands = self.scene:attach(hand_mesh, self.scene:active_camera())

    -- HUD elements?
end

function client:render()
    self.scene:render()
end

return client
