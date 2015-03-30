
# Copyright (C) 2014 Luke San Antonio
# All rights reserved.

function(add_data target all_or_nah)
  add_custom_target(${target} ${all_or_nah})

  foreach(filename ${ARGN})
    add_custom_command(TARGET ${target} COMMAND ${CMAKE_COMMAND}
                       ARGS -E copy_if_different
                       ${CMAKE_CURRENT_SOURCE_DIR}/${filename}
                       ${CMAKE_CURRENT_BINARY_DIR}/${filename}
                       MAIN_DEPENDENCY ${filename})
    install(FILES ${filename} DESTINATION share/${PROJECT_NAME})
  endforeach(filename)
endfunction()
