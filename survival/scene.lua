
-- Copyright (c) 2016 Luke San Antonio
-- All rights reserved.

local scene = {}

function scene.make_scene(eng)
  local sc = {}

  function sc:add_camera(tp)
  end

  function sc:active_camera(tp)
  end

  -- A transformation applied to a mesh only really makes sense in the context
  -- of a scene!
  function sc:attach(mesh, parent)
    if(parent) then
    end
  end

  function sc:render()
  end

  return sc
end
