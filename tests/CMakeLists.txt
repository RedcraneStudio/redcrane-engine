
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

macro(add_tests subdirectory)
  foreach(filename ${ARGN})
    set(RED_CRANE_TEST_FILES ${RED_CRANE_TEST_FILES} "${subdirectory}/${filename}")
  endforeach()
endmacro()

add_tests(common tree.cpp volume.cpp value_map.cpp)
add_executable(run_all_tests main.cpp ${RED_CRANE_TEST_FILES})

target_include_directories(run_all_tests PUBLIC ${CMAKE_SOURCE_DIR}
                           ${CMAKE_CURRENT_SOURCE_DIR}/../src)
target_link_libraries(run_all_tests PUBLIC commonlib)
