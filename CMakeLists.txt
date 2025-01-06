cmake_minimum_required(VERSION 3.24)
project(LegacyCubiomes)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


include_directories(${CMAKE_SOURCE_DIR})

# used for debugging statements
add_definitions(-DDEV_MODE)

# this is used for including QT related debug code.
# add_definitions(-DINCLUDE_QT)

file(GLOB_RECURSE LEGACY_CUBIOMES LegacyCubiomes/*)

file(GLOB_RECURSE LCE_RESOURCES lce/*)

add_compile_options(-mavx2 -mpopcnt)

add_executable(LegacyCubiomes
        ${LEGACY_CUBIOMES}
        ${LCE_RESOURCES}
        # programs/gen_chunk.cpp
        programs/mine_a_chunk_ravines.cpp
)
