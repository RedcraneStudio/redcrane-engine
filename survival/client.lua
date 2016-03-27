
-- Copyright (c) 2016 Luke San Antonio
-- All rights reserved.

local rc = require("redcrane")

local client = {}

function client:init()
    -- TODO: Move engine init here, and share stuff between client and server

    -- What scene shall we use?
    self.scene = rc.scene.make_scene(rc.engine)

    -- Use an fps camera, it's the only one so it will be made active.
    self.scene:add_camera("fps")

    -- Load in the character's hands
    self.mesh_pool = rc.mesh_pool.make_mesh_pool(rc.engine)
    local hand_mesh = self.mesh_pool:load_mesh("character/hands")

    -- Attach the hands to the camera
    --self.hands = self.scene:attach(hand_mesh, self.scene:active_camera())
    self.hands = self.scene:attach(hand_mesh)

    -- HUD elements?
end

client:init()
rc:log_i("Done initializing")

while rc:running() do
    client.scene:step()
    client.scene:render()
    rc:swap_window()
end

return 0
