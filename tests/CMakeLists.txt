
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

macro(add_tests subdirectory)
  foreach(filename ${ARGN})
    set(REDC_TEST_FILES ${REDC_TEST_FILES} "${subdirectory}/${filename}")
  endforeach()
endmacro()

add_tests(common cache.cpp id_gen.cpp tree.cpp utility.cpp value_map.cpp
                 vector.cpp volume.cpp)
add_tests(terrain chunks.cpp)
add_tests(rpc msgpack_plugin.cpp)
add_executable(run_all_tests main.cpp ${REDC_TEST_FILES})

target_include_directories(run_all_tests PUBLIC ${CMAKE_SOURCE_DIR}
                           ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(run_all_tests PUBLIC commonlib opensimplex terrainlib
                                           rpclib)
