-- Copyright (C) 2015 Luke San Antonio
-- Released under the 3-clause BSD License found in the mod root (LICENSE.txt)

terrain_gen = require('terrain_gen')

rc = require('redcrane')

rc.register_terrain_landmass_func(terrain_gen, {
  octaves = 8
});
