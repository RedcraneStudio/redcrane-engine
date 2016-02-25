
-- Copyright (c) 2016 Luke San Antonio
-- All rights reserved.

local ffi = require("ffi")

ffi.cdef[[
  void *redc_load_mesh(void *engine, const char *str);
  void redc_draw_mesh(void *engine, void *msh);
]]

local mesh_pool = {}

-- So far no engine support, so we just load individual meshes
function mesh_pool.make_mesh_pool(eng)
  local mp = {}

  function mp:load_mesh(str)
    local mesh = ffi.C.load_mesh(eng, str)

    mesh.render = function(self)
      ffi.C.draw_mesh(eng, self)
    end

    return mesh
  end

  return mp
end

return mesh_pool
