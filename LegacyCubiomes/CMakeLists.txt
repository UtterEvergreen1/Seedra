cmake_minimum_required(VERSION 3.24)
project(LegacyCubiomesLib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Gather all the .cpp/.hpp for LegacyCubiomes
file(GLOB_RECURSE LEGACY_CUBIOMES_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
        ...
)

add_library(LegacyCubiomesLib STATIC ${LEGACY_CUBIOMES_SOURCES}
        building_blocks/StructureVariant.hpp)

# Link the lce library
target_link_libraries(LegacyCubiomesLib PRIVATE lceLib)

# If your code does #include "LegacyCubiomes/xyz.hpp",
# you need the parent of this folder on the path:
target_include_directories(LegacyCubiomesLib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

# Additional compile flags
add_definitions(-DDEV_MODE)
add_compile_options(-mavx2 -mpopcnt)

# (Optional) If you want a standalone LegacyCubiomesExe for testing:
option(LC_BUILD_EXECUTABLE "Build LegacyCubiomes standalone executable" OFF)
if(LC_BUILD_EXECUTABLE)
    add_executable(LegacyCubiomesExe
            ${LEGACY_CUBIOMES_SOURCES}
            structures/gen/Structure.cpp
            structures/gen/Structure.hpp
            # or a main file
    )
    target_link_libraries(LegacyCubiomesExe PRIVATE LegacyCubiomesLibs)
endif()
