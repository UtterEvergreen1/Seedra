cmake_minimum_required(VERSION 3.24)
project(LCE_Cubiomes)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)

# This is used for testing java loot tables and enchantments.
# add_definitions(-DINCLUDE_JAVA)

# this is used for including QT related debug code.
# add_definitions(-DINCLUDE_QT)

file(GLOB_RECURSE LEGACY_CUBIOMES LegacyCubiomes/*)
add_executable(LCE_Cubiomes "main1.cpp" ${LEGACY_CUBIOMES})

