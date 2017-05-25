
# Copyright (C) 2017 Luke San Antonio Bialecki
# All rights reserved.

find_path(Sodium_INCLUDE_DIR sodium.h)
find_library(Sodium_LIBRARY sodium)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sodium DEFAULT_MSG Sodium_INCLUDE_DIR
        Sodium_LIBRARY)

