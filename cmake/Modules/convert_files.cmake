
# Copyright (C) 2014 Luke San Antonio
# All rights reserved.

function(convert_to_png target dir in_file out_file)
  # Create necessary directories
  add_custom_command(TARGET ${target} PRE_BUILD COMMAND
                     ${CMAKE_COMMAND} -E make_directory
                     ${CMAKE_CURRENT_BINARY_DIR}/${dir})

  # Find imagemagick
  find_package(ImageMagick COMPONENTS Magick++)
  # find_package(ImageMagick COMPONENTS convert REQUIRED)
  # Actually convert the image.
  add_custom_command(TARGET ${target} POST_BUILD COMMAND
                     ${ImageMagick_convert_EXECUTABLE}
                     -background none
                     ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${in_file}
                     -format png ${out_file}
                     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${dir})
endfunction()
