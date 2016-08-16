
-- Copyright (c) 2016 Luke San Antonio
-- All rights reserved.

local rc = redcrane

local decl = {
    map = "Library-map",
    properties = "properties",
    skin = "default_skin"
}

scene = rc.scene.make_scene()
player = scene:add_player()

map = rc:load_map(decl.map)

-- Initialize custom components
charge_carrier = rc:make_component()
charge_carrier.charge = 0.0
charge_carrier.decay_rate = 0.0

-- Build a set of (fully charged) batteries
batteries = {}
for i=1,8 do
    batteries[i] = rc:make_entity()
    batteries[i].add_component(charge_carrier)
    batteries[i].add_render_component("Battery_asset")
    batteries[i].add_physics_component("rigidbody")

    batteries[i].charge = 1.0
    batteries[i].decay_rate = 0.0
end

-- Build the grappling gun
gun == rc:make_entity()
gun.add_component(charge_carrier)
gun.add_render_component("Gun_asset")

gun.charge = 0.0

-- Figure out behavior of decay rate
gun.decay_rate = 1.0

rc:log_i("Done initializing")

while rc:running() do
    rc:step()
    scene:step()
    scene:render()
    rc:swap_window()
end

return 0
