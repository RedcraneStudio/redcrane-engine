
-- Copyright (C) 2016 Luke San Antonio
-- All rights reserved.

-- Standard no-frills sandbox for the configuration file

-- Making the sandbox environment in Lua is so much easier because we have to
-- make sure we don't mutate the global environment and properly references
-- variables that *are* in the global environment.

-- Return a function that builds the sandbox
return function ()
    local require_cache = {}
    return {
        ipairs = ipairs,
        error = error,
        next = next,
        pairs = pairs,
        pcall = pcall,
        select = select,
        tonumber = tonumber,
        tostring = tostring,
        type = type,
        unpack = unpack,
        xpcall = xpcall,
        _VERSION = _VERSION,
        coroutine = {
            create = coroutine.create,
            resume = coroutine.resume,
            running = coroutine.running,
            status = coroutine.status,
            wrap = coroutine.wrap
        },
        string = {
            byte = string.byte,
            char = string.char,
            find = string.find,
            format = string.format,
            gmatch = string.gmatch,
            gsub = string.gsub,
            len = string.len,
            lower = string.lower,
            match = string.match,
            rep = string.rep,
            reverse = string.reverse,
            sub = string.sub,
            upper = string.upper
        },
        table = {
            insert = table.insert,
            maxn = table.maxn,
            remove = table.remove,
            sort = table.sort
        },
        math = {
            abs = math.abs,
            acos = math.acos,
            asin = math.asin,
            atan = math.atan,
            atan2 = math.atan2,
            ceil = math.ceil,
            cos = math.cos,
            cosh = math.cosh,
            deg = math.deg,
            exp = math.exp,
            floor = math.floor,
            fmod = math.fmod,
            frexp = math.frexp,
            huge = math.huge,
            ldexp = math.ldexp,
            log = math.log,
            log10 = math.log10,
            max = math.max,
            min = math.min,
            modf = math.modf,
            pi = math.pi,
            pow = math.pow,
            rad = math.rad,
            random = math.random,
            randomseed = math.randomseed,
            sin = math.sin,
            sinh = math.sinh,
            sqrt = math.sqrt,
            tan = math.tan,
            tanh = math.tanh
        },
        os = {
            clock = os.clock,
            difftime = os.difftime,
            time = os.time
        },
        require = function(name)
            -- Cache the file
            filename = name..".lua"
            if require_cache[filename] == nil then
                local func, err = loadfile(filename)
                if func == nil then
                    error(err)
                else
                    require_cache[filename] = func()
                end
            end
            return require_cache[filename]
        end
    }
end
