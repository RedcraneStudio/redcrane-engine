
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

-- Seed the random number generator
math.randomseed(os.time())

local GENERATOR_LOADS = {
    [1] = {
        base_load = 0.00,
        vary_fac = 0.0,
    },
    [2] = {
        base_load = 0.05,
        vary_fac = 0.05,
    },
    [3] = {
        base_load = 0.10,
        vary_fac = 0.07,
    },
    [4] = {
        base_load = 0.15,
        vary_fac = 0.10,
    },
    [5] = {
        base_load = 0.20,
        vary_fac = 0.1,
    },
    [6] = {
        base_load = 0.25,
        vary_fac = 0.2,
    },
    [7] = {
        base_load = 0.30,
        vary_fac = 0.3,
    },
    [8] = {
        base_load = 0.35,
        vary_fac = 0.3,
    },
    [9] = {
        base_load = 0.40,
        vary_fac = 0.3,
    },
    [10]= {
        base_load = 0.45,
        vary_fac = 0.4,
    },
    [11]= {
        base_load = 0.5,
        vary_fac = 0.5,
    },
    [11]= {
        base_load = 1.0,
        vary_fac = 1.0,
    },
}

local tk = rc:make_timekeeper()
local message_stream = rc.text.make_text_stream(2.0)
tk:add(message_stream)

local gen = require('generator')
generator = gen.load_controller(GENERATOR_LOADS, 1, rc:cur_time())

local step_timer = rc:make_timer()
step_timer:set_steady_timeout(1)
step_timer:set_callback(function (timer)
        generator:inc_load()
        timer:reset()
end)
step_timer:reset()

rc:log_i("Done initializing")

function power_at_load(load_val)
    -- The (input) value can be greater than one but not smaller than zero.
    -- Handle the case where we get a negative number (we can't have a negative
    -- light intensity, so just use zero).
    return math.max(0.0, 1.0 - load_val)
end


while rc:running() do
    rc:step()
    scene:step()
    tk:step()

    for event in rc:events() do
        if not map:check_loaded(event) then
            rc:log_d("event (type:", event.type, "name:", event.name, "data:", event.data)
            if event.type == "physics" and event.name == "desk_lamp_toggle" then
                local state = map:get_light_state("Desk_Lamp")

                -- Toggle the light
                if state.power < 0.5 then
                    state.power = 1.0
                else
                    state.power = 0.0
                end
                map:set_light_state("Desk_Lamp", state)

                message_stream:push_string("power: "..state.power)
            end
        end
    end

    for light_i = 0, map:get_num_lights() - 1 do
        local light_state = map:get_light_state(light_i)

        local cur_load = generator:current_load(rc:cur_time())
        light_state.power = power_at_load(cur_load)

        map:set_light_state(light_i, light_state)
    end

    -- render the scene
    scene:render()

    -- draw the hud
    rc.text.draw(message_stream:full_text())

    rc:swap_window()
    rc:ms_sleep(2)
end

coroutine.yield(0)
