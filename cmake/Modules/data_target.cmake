
# Copyright (C) 2014 Luke San Antonio
# All rights reserved.

function(add_data target all_or_nah destination)
  add_custom_target(${target} ${all_or_nah})

  foreach(filename ${ARGN})
    add_custom_command(TARGET ${target} COMMAND ${CMAKE_COMMAND}
                       ARGS -E copy_if_different
                       ${CMAKE_CURRENT_SOURCE_DIR}/${filename}
                       ${CMAKE_CURRENT_BINARY_DIR}/${filename}
                       MAIN_DEPENDENCY ${filename})
    get_filename_component(SUBDIR ${filename} DIRECTORY)
    install(FILES ${filename} DESTINATION ${destination}/${SUBDIR})
  endforeach(filename)
endfunction()

function(add_data_directory target all)
  add_custom_target(${target} ${all})

  foreach(dirname ${ARGN})
    add_custom_command(TARGET ${target} COMMAND ${CMAKE_COMMAND}
                       ARGS -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/${dirname}
                       ${CMAKE_CURRENT_BINARY_DIR}/${dirname}
                       MAIN_DEPENDENCY ${dirname})
    install(DIRECTORY ${dirname} DESTINATION share/${PROJECT_NAME})
  endforeach()
endfunction()
