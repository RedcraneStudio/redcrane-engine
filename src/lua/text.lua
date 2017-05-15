local ffi = require('ffi')

local rc = require('redcrane')

local M = {}

function M.draw(text, pt)
    return ffi.C.redc_text_draw(rc.engine, text)
end

return M