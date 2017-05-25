
# Copyright (C) 2017 Luke San Antonio Bialecki
# All rights reserved.

find_path(Yojimbo_INCLUDE_DIR yojimbo.h)
find_library(Yojimbo_LIBRARY yojimbo)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Yojimbo DEFAULT_MSG Yojimbo_INCLUDE_DIR
        Yojimbo_LIBRARY)

