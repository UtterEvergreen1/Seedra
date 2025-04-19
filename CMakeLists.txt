cmake_minimum_required(VERSION 3.24)
project(LIB_LegacyCubiomes)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Build lceLib from the submodule
add_subdirectory(lce)

# Gather all the .cpp/.hpp for LegacyCubiomes
file(GLOB_RECURSE LEGACY_CUBIOMES_SOURCES
        # submodule
        "${CMAKE_CURRENT_LIST_DIR}/lce/*.cpp"
        # project
        "${CMAKE_CURRENT_LIST_DIR}/components/*.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/chunk_detector/*.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/terrain/*.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/cubiomes/*.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/enchants/*.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/include/*.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/loot/*.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/structures/*.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/common/*.cpp"
        ...
)

add_library(LIB_LegacyCubiomes STATIC ${LEGACY_CUBIOMES_SOURCES})
target_compile_features(LIB_LegacyCubiomes PRIVATE cxx_std_20)


# Link the lce library
target_link_libraries(LIB_LegacyCubiomes PRIVATE LIB_lce)

# If your code does #include "LegacyCubiomes/xyz.hpp",
# you need the parent of this folder on the path:
target_include_directories(LIB_LegacyCubiomes
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
)

