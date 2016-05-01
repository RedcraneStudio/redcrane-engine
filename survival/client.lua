
-- Copyright (c) 2016 Luke San Antonio
-- All rights reserved.

local rc = redcrane

function dump(tab, prefix)
    prefix = prefix or ''
    if type(tab) == 'table' then
        for key, value in pairs(tab) do
            rc:log_i(prefix, key, " ", value)
            if type(value) == 'table' then
                dump(value, prefix..'\t')
            end
        end
    end
end
dump(rc)

local client = {}

function client:init()
    -- What scene shall we use?
    self.scene = rc.scene.make_scene()

    self.player = self.scene:add_player()

    self.shaders = {}
    self.shaders.basic = rc.shader.load_shader("basic");
    self.shaders.envmap = rc.shader.load_shader("envmap");
    self.shaders.hud = rc.shader.load_shader("hud");

    self.map = rc:load_map("map")

    -- HUD elements?
end

client:init()
rc:log_i("Done initializing")

while rc:running() do
    rc:step()
    client.scene:step()
    client.scene:render()
    rc:swap_window()
end

return 0
