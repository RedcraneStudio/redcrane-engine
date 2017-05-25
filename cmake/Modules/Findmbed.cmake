
# Copyright (C) 2017 Luke San Antonio Bialecki
# All rights reserved.

find_path(mbed_INCLUDE_DIR mbedtls.h)
find_library(mbedtls_LIBRARY mbedtls)
find_library(mbedcrypto_LIBRARY mbedcrypto)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(mbed DEFAULT_MSG mbed_INCLUDE_DIR
        mbedtls_LIBRARY mbedcrypto_LIBRARY)

