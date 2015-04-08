
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

add_executable(strat_test_suite maybe_owned.cpp mesh.cpp grid_iterator.cpp)
target_link_libraries(strat_test_suite commonlib)
