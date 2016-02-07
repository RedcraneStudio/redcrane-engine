
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

find_path(ENet_INCLUDE_DIR enet/enet.h)
find_library(ENet_LIBRARY enet)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ENet DEFAULT_MSG ENet_INCLUDE_DIR ENet_LIBRARY)

