
-- Copyright (c) 2016 Luke San Antonio
-- All rights reserved.

local rc = redcrane

local decl = {
    map = "Office-map",
    properties = "properties",
    skin = "default_skin"
}

scene = rc.scene.make_scene()
player = scene:add_player()

map = rc:load_map(decl.map)

rc:log_i("Done initializing")

while rc:running() do
    rc:step()
    scene:step()

    for event in rc:events() do
        if event.name == "desk_lamp_toggle" then
            rc:log_i("Desk Lamp toggle ", event)
        end
    end

    scene:render()
    rc:swap_window()

end

return 0
