add_executable(survive main.cpp)

# For now use hardcoded paths, but later add a find module.
set(IRRLICHT_LIBRARIES /usr/lib/libIrrlicht.so CACHE FILEPATH "")
set(IRRLICHT_INCLUDE_DIRS /usr/include/irrlicht/ CACHE PATH "")

target_link_libraries(survive ${IRRLICHT_LIBRARIES})
target_include_directories(survive PUBLIC ${IRRLICHT_INCLUDE_DIRS})
