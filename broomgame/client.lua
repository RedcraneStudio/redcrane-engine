
-- Copyright (c) 2016 Luke San Antonio
-- All rights reserved.

local rc = redcrane

local decl = {
    map = "Broomgame-map",
}

scene = rc.scene.make_scene()
player = scene:add_player()

local msh = rc.mesh:load_mesh("Broomstick")
local obj = scene:add_mesh(msh)
scene:set_parent(obj, player.camera)

map = rc:load_map(decl.map)


rc:log_i("Done initializing")

while rc:running() do
    rc:step()
    scene:step()
    scene:render()
    rc:swap_window()
    rc:ms_sleep(2)
end

return 0
