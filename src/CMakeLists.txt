add_executable(survive main.cpp)

find_package(Irrlicht REQUIRED)
target_link_libraries(survive ${Irrlicht_LIBRARY})
target_include_directories(survive PUBLIC ${Irrlicht_INCLUDE_DIR})
