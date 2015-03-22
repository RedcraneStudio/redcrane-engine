add_executable(survive main.cpp)

find_package(GLFW REQUIRED)
target_link_libraries(survive ${GLFW_LIBRARY})
target_include_directories(survive PUBLIC ${GLFW_INCLUDE_DIR})

target_include_directories(survive PUBLIC ${CMAKE_SOURCE_DIR})
