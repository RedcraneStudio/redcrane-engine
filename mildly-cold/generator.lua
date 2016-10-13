
local noise = require("lj_noise")

local M = {}

local function triangle_fn(x, y)
    local freq = 5.0
    return (8.0 / math.pi) * (math.sin(2 * math.pi * freq * x) - 1.0 / 9.0 * math.sin(6 * math.pi * freq * x) + 1.0 / 25.0 * math.sin(10 * math.pi * freq *x))
end

local function noise_fn(x, y)
    return noise.Simplex2D(x, y)
end

--- Returns a generator / iterator function to generate load values for the
--- generator.
-- @param base_load How overloaded the engine is passively, in the
-- range [0.0, 1.0]
-- @param vary_fac The tendencity of the engine to shit the bed, in the range
-- [0.0, 1.0]
function M.varying_load_fn(base_load, vary_fac)
    -- For every 10th of vary_fac, add another noise line that could potentially
    -- make the engine shit the bed in some way.
    local lines = math.floor(vary_fac / .10)

    -- Accumulates time. By doing it this way we guarantee (sorta) that we'll
    -- only move forward, which is important to maintain a sane state. They
    -- could always pass in a very large dt or a negative dt but whatever.
    local accum_time = 0.0

    -- No current line
    local cur_line = -1
    return function(dt)
        accum_time = accum_time + dt

        -- No current line, find the one that, at this point, has the highest
        -- value.
        if cur_line == -1 then
            -- Records the value and the line we found it on.
            local value = base_load
            local line_i = -1

            for try_i = 0,lines do
                -- We're sorta exploring parallel lines of noise, the more lines
                -- we check, the more likely we can use one to make the engine
                -- feel more unstable.
                local noise_res = triangle_fn(accum_time, try_i)
                if noise_res > value then
                    value = noise_res
                    line_i = try_i
                end
            end

            -- Change to this line. If we didn't find one larger than base_load,
            -- cur_line and line_i will remain -1, so we'll continue searching
            -- at a later time.
            cur_line = line_i

            -- Return its current value (already calculated).
            return value
        else
            -- Get the current value on the current line, if it's below the base
            -- level we return the base level and go back to searching each
            -- line.
            local noise_res = triangle_fn(accum_time, try_i)
            if noise_res < base_load then
                -- Forget about this line and return to the base load.
                cur_line = -1
                return base_load
            else
                -- Return noise value on this line.
                return noise_res
            end
        end
    end
end

function M.constant_load_fn(base_load)
    return function(dt)
        return base_load
    end
end

-- Handles the load controller of the engine.
-- @param load_config A list of options for each possible state of the
-- generator.
-- @param cur_load_i Index of the initial load config of the generator.
function M.load_controller(load_config, init_load_i, init_time)
    local C = {
        cur_load_i = init_load_i,
        last_time = init_time,
    }

    function C:_update_controller()
        local cur_config = load_config[self.cur_load_i]
        self.controller = M.varying_load_fn(cur_config.base_load,
                                            cur_config.vary_fac)
    end

    -- Set the initial controller.
    C:_update_controller()

    function C:inc_load()
        if self.cur_load_i < #load_config then
            self.cur_load_i = self.cur_load_i + 1
        end
        self:_update_controller()
    end
    function C:dec_load()
        if self.cur_load_i > 1 then
            self.cur_load_i = self.cur_load_i - 1
        end
        self:_update_controller()
    end

    function C:current_load(time)
        local dt = time - self.last_time
        self.last_time = time
        return self.controller(dt)
    end

    return C
end

return M
