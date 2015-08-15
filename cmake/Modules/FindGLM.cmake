
# Copyright (C) 2015 Luke San Antonio and Hazza Alkaabi
# All rights reserved.

find_path(GLM_INCLUDE_DIR glm/glm.hpp)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLM DEFAULT_MSG GLM_INCLUDE_DIR)
