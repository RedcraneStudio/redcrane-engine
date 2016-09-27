
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
        if not map:check_loaded(event) then
            rc:log_i("event", event.type, event.name, event.data)
            if event.type == "physics" and event.name == "desk_lamp_toggle" then
                local state = map:get_light_state("Spot")
                rc:log_i("Physics event")
                map:set_light_state("Spot", { on = not state.on })
             end
        end
    end

    scene:render()
    rc:swap_window()

end

return 0
