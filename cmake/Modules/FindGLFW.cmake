find_library(GLFW_LIBRARY NAMES glfw3 glfw)
find_path(GLFW_INCLUDE_DIR glfw3.h PATH_SUFFIXES GLFW)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLFW GLFW_LIBRARY GLFW_INCLUDE_DIR)
