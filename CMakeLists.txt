cmake_minimum_required(VERSION 3.24)
project(LCE_Cubiomes)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)

set(Boost_INCLUDE_DIR "C:/boost_1_83_0")
set(BOOST_ROOT "C:/boost_1_83_0")
find_package(Boost REQUIRED)

if(Boost_FOUND)

    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})

endif()

# used for debugging statements
add_definitions(-DDEV_MODE)

# This is used for testing java loot tables and enchantments.
# add_definitions(-DINCLUDE_JAVA)

# this is used for including QT related debug code.
# add_definitions(-DINCLUDE_QT)

# recursively grabs all files inside the project folder
file(GLOB_RECURSE LEGACY_CUBIOMES LegacyCubiomes/*)
add_executable(LCE_Cubiomes "main1.cpp" ${LEGACY_CUBIOMES})

if(Boost_FOUND)
    target_link_libraries(LCE_Cubiomes ${Boost_LIBRARIES})
endif()

