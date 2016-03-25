
-- Copyright (c) 2016 Luke San Antonio
-- All rights reserved.

local ffi = require("ffi")

ffi.cdef[[
  void *redc_load_mesh(void *engine, const char *str);
  void redc_unload_mesh(void *mesh);
]]

local mesh_pool = {}

-- So far no engine support, so we just load individual meshes
function mesh_pool.make_mesh_pool(eng)
  local mp = {}

  function mp:load_mesh(str)
    return ffi.gc(ffi.C.redc_load_mesh(eng, str), ffi.C.redc_unload_mesh)
  end

  return mp
end

return mesh_pool
