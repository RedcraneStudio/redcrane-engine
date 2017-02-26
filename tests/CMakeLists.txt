
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

macro(add_tests subdirectory)
  foreach(filename ${ARGN})
    set(REDC_TEST_FILES ${REDC_TEST_FILES} "${subdirectory}/${filename}")
  endforeach()
endmacro()

add_tests(common algorithm.cpp cache.cpp circle.cpp grid_iterator.cpp
                 id_gen.cpp maybe_owned.cpp tree.cpp utility.cpp value_map.cpp
                 vec.cpp volume.cpp peer_ptr.cpp timed_text_test.cpp)

add_tests(gfx mesh.cpp)

add_executable(run_all_tests main.cpp ${REDC_TEST_FILES})

target_include_directories(run_all_tests PUBLIC ${CMAKE_SOURCE_DIR}
                           ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(run_all_tests PUBLIC commonlib opensimplex
                                           gfxlib gfxextralib)
