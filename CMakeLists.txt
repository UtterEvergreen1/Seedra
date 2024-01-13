cmake_minimum_required(VERSION 3.24)
project(LCE_Cubiomes)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)

# used for debugging statements
add_definitions(-DDEV_MODE)

# this is used for including QT related debug code.
# add_definitions(-DINCLUDE_QT)


# recursively grabs all files inside the project folder
file(GLOB_RECURSE LEGACY_CUBIOMES LegacyCubiomes/*)
add_executable(LCE_Cubiomes "programs/draw_images.cpp" ${LEGACY_CUBIOMES}
        _working_on/UnderwaterLargeCaveGenerator.hpp
        _working_on/UnderwaterCanyonGenerator.hpp)