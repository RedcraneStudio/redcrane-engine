
-- Copyright (c) 2015 Luke San Antonio
-- All rights reserved.

local ffi = require("ffi")

ffi.cdef[[
    typedef struct
    {
        const char* window_title;
    } Redc_Config;

    void* redc_init_engine(Redc_Config cfg);
    void redc_uninit_engine(void* eng);
    bool redc_running(void* eng);
    void redc_step(void* eng);
]]

local rc = {}

-- TODO: Potentially clean up the config
local config = require("config")

rc.engine = ffi.gc(ffi.C.redc_init_engine(config), ffi.C.redc_uninit_engine)

rc.scene = require("scene")
rc.mesh_pool = require("mesh_pool")

function rc:running()
    ffi.C.redc_running(rc.engine)
end
function rc:step()
    ffi.C.redc_step(rc.engine)
end

return rc
