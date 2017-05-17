local ffi = require('ffi')

local rc = require('redcrane')

local M = {}

function M.draw(text, pt)
    pt = pt or {x = 0.0, y = 0.0}
    return ffi.C.redc_text_draw(rc.engine, text, pt.x, pt.y)
end

function M.make_text_stream(timeout)
    timeout = timeout or 1.0

    local stream = {}
    stream._ptr = ffi.gc(ffi.C.redc_text_stream_new(rc.engine, timeout),
                         ffi.C.redc_text_stream_delete)

    function stream:push_string(text)
        ffi.C.redc_text_stream_push_string(self._ptr, text)
    end

    function stream:full_text()
        -- This copies the string twice. We wouldn't have to do that if we
        -- could get a pointer to the text stream data directly, instead of
        -- having to copy the temporary and then copy it again to make it a lua
        -- string.
        local str = ffi.C.redc_text_stream_full_text(self._ptr)
        local ret = ffi.string(str)
        ffi.C.free(ffi.cast('void*', str))

        return ret
    end

    function stream:step(dt)
        ffi.C.redc_text_stream_step(self._ptr, dt)
    end

    return stream
end


return M