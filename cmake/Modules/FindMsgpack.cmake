
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

find_path(Msgpack_INCLUDE_DIR msgpack.hpp)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Msgpack DEFAULT_MSG Msgpack_INCLUDE_DIR)
