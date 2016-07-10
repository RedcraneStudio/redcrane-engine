
-- Copyright (c) 2016 Luke San Antonio
-- All rights reserved.

local rc = redcrane

local decl = {
    map = "Library",
    properties = "properties",
    skin = "default_skin"
}

rc:load_game_set(decl)

while rc:running() do
    rc:step()
    rc:render()
    rc:swap_window()
end

return 0
