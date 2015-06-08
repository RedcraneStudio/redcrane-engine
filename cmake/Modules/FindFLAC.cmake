
# Copyright (C) 2015 Luke San Antonio
# All rights reserved.

find_library(FLAC_LIBRARY FLAC)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FLAC DEFAULT_MSG FLAC_LIBRARY)
