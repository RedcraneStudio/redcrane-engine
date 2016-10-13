
-- Copyright (c) 2015 Luke San Antonio
-- All rights reserved.

local ffi = require("ffi")

local rc = {}

function rc:init(config)
    self.engine = self.engine or ffi.gc(ffi.C.redc_init_engine(config),
                                        ffi.C.redc_uninit_engine)

    self.scene = require("scene")
    self.mesh = require("mesh")
    self.texture = require("texture")
    self.shader = require("shader")
    self.server = require("server")

    self.timers = {}

    return self.engine
end

function rc:init_client()
    ffi.C.redc_init_client(self.engine)
end
function rc:init_server()
    ffi.C.redc_init_server(self.engine)
end

function rc:step()

    -- Check timers and activate callback if necessary
    for _, timer in pairs(self.timers) do
        timer:_check_done()
    end

    return ffi.C.redc_step_engine(self.engine)
end

function rc:render()
    return ffi.C.redc_render_engine(self.engine)
end

function rc:running()
    return ffi.C.redc_running(self.engine)
end
function rc:swap_window()
    ffi.C.redc_window_swap(self.engine)
end

function rc:asset_path()
    -- This memory doesn't have to be freed
    return ffi.C.redc_get_asset_path(self.engine)
end

function rc:load_map(filename)
    ffi.C.redc_map_load(self.engine, filename)

    local map = {}
    -- A map doesn't exist yet
    map.ptr_ = nil

    function map:_check_loaded()
        if self.ptr_ == nil then
            error("Map doesn't exist yet")
        end
    end
    function map:get_num_lights()
        self:_check_loaded()
        return tonumber(ffi.C.redc_map_get_num_lights(self.ptr_))
    end

    function map:get_light_state(light)
        self:_check_loaded()
        if type(light) == "string" then
            return ffi.C.redc_map_get_light_state(self.ptr_, light)
        else
            return ffi.C.redc_map_get_light_i_state(self.ptr_, light)
        end
    end
    function map:set_light_state(light, state)
        self:_check_loaded()
        local res
        if type(light) == "string" then
            res = ffi.C.redc_map_set_light_state(self.ptr_, light, state)
        else
            res = ffi.C.redc_map_set_light_i_state(self.ptr_, light, state)
        end
        -- TODO: error code?
        if res == 0 then
            error("Failed to set state of light '"..light.."'")
        end
    end
    function map:check_loaded(event)
        if event.type == "map_loaded" then
            self.ptr_ = event.data
            return true
        else
            return false
        end
    end
    return map
end
function rc:load_hud(filename)
    return ffi.C.redc_hud_load(self.engine, filename)
end
function rc:load_skin(filename)
    return ffi.C.redc_skin_load(self.engine, filename)
end

function rc:load_game_set(set)
    if set.map then
        rc:load_map(set.map)
    end
    if set.hud then
        rc:load_hud(set.hud)
    end
    if set.skin then
        rc:load_skin(set.skin)
    end
end

function rc:events()
    event_data = ffi.new("Redc_Event")
    return function()
        if ffi.C.redc_server_poll_event(self.engine, event_data) ~= 0 then
            -- @ Optimization: If somehow the string copying becomes a
            -- bottleneck, have a function to only iterate over events with a
            -- specific type and do the string comparison in C++ code.
            return { type = ffi.string(event_data.type),
                     name = ffi.string(event_data.name),
                     data = event_data.data}
        else
            return nil
        end
    end
end

-- Logging functions
function stringify(str, ...)
    for n=1,select('#', ...) do
        local e = select(n, ...)
        str = str .. ' ' .. tostring(e)
    end
    return str
end

rc.internal = {}

function rc.internal:log_d(str, ...)
    ffi.C.redc_log_d(stringify(tostring(str), ...))
end
function rc.internal:log_i(str, ...)
    ffi.C.redc_log_i(stringify(tostring(str), ...))
end
function rc.internal:log_w(str, ...)
    ffi.C.redc_log_w(stringify(tostring(str), ...))
end
function rc.internal:log_e(str, ...)
    ffi.C.redc_log_e(stringify(tostring(str), ...))
end

local prefix = "(Mod) "
function rc:log_d(str, ...)
    rc.internal:log_d(prefix, str, ...)
end
function rc:log_i(str, ...)
    rc.internal:log_i(prefix, str, ...)
end
function rc:log_w(str, ...)
    rc.internal:log_w(prefix, str, ...)
end
function rc:log_e(str, ...)
    rc.internal:log_e(prefix, str, ...)
end

-- Find a better place for this function, right now we take a reference to self,
-- even though we don't need it
function rc:ms_sleep(ms)
    ffi.C.redc_ms_sleep(ms)
end

function rc:cur_time()
    return ffi.C.redc_cur_time()
end

function rc:make_timer()
    local T = {
        min = 1.0,
        max = 1.0,
        last_reset = rc:cur_time(),
        cur_duration = 1.0,
        running = true,
        callback = function() end
    }

    function T:set_random_timeout(min, max)
        self.min = min
        self.max = max
        self:_calc_duration()
    end
    function T:set_steady_timeout(sec)
        self.min = sec
        self.max = sec
        self:_calc_duration()
    end

    function T:set_callback(fn)
        self.callback = fn
    end

    function T:reset()
        self.running = true
        self.last_reset = rc:cur_time()

        self:_calc_duration()
    end

    function T:_calc_duration()
        if self.min == self.max then
            self.cur_duration = self.min
        end

        -- If it's random, come up with something on the spot
        self.cur_duration = math.random() * (self.max - self.min) + self.min
    end


    function T:_check_done()
        local now = rc:cur_time()
        if now - self.last_reset > self.cur_duration and self.running then
            -- Stop before the callback as it may reset the timer
            self.running = false
            self.callback(self)
        end
    end

    table.insert(self.timers, T)
    return T
end

return rc
