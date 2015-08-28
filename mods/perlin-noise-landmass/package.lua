-- Copyright (C) 2015 Luke San Antonio
-- Released under the 3-clause BSD License found in the mod root (LICENSE.txt)

terrain_gen = require('terrain_gen')

terrain = {}
terrain.landmass = {
    gen = terrain_gen,
    config = {
      octaves = 8
    }
}
