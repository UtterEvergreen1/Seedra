cmake_minimum_required(VERSION 3.24)
project(LIB_LegacyCubiomes)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -s")
set(CMAKE_C_FLAGS_DEBUG  "${CMAKE_C_FLAGS_DEBUG} -O3")
set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} -O3")

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


# Link the lce library
target_link_libraries(LIB_LegacyCubiomes PRIVATE LIB_lce)

# If your code does #include "LegacyCubiomes/xyz.hpp",
# you need the parent of this folder on the path:
target_include_directories(LIB_LegacyCubiomes
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
)


# (Optional) If you want a standalone LegacyCubiomesExe for testing:
unset(LC_BUILD_EXECUTABLE CACHE)
option(LC_BUILD_EXECUTABLE "Build LegacyCubiomes standalone executable" ON)
if(LC_BUILD_EXECUTABLE)

    # Chunk Gen Code For LegacyChunkViewer
    add_executable(EXE_GenCreateWorld "tests/gen_chunk.cpp")
    target_link_libraries(EXE_GenCreateWorld PRIVATE LIB_LegacyCubiomes)

    # Chunk Gen Code For LegacyChunkViewer
    add_executable(EXE_GetMaxTempBiomes "tests/get_min_temp_biomes.cpp")
    target_link_libraries(EXE_GetMaxTempBiomes PRIVATE LIB_LegacyCubiomes)

    # Ravine Gen Code for finding best seed for mine a chunk
    add_executable(EXE_MineAChunkSF "tests/best_mine_chunk_seed.cpp")
    target_link_libraries(EXE_MineAChunkSF PRIVATE LIB_LegacyCubiomes)

    # Test Village generation speed
    add_executable(EXE_TestVillageGen "tests/test_village_gen_speed.cpp")
    target_link_libraries(EXE_TestVillageGen PRIVATE LIB_LegacyCubiomes)


    # Test Enchants
    add_executable(EXE_TestLootEnchants "tests/test_loot_enchants.cpp")
    target_link_libraries(EXE_TestLootEnchants PRIVATE LIB_LegacyCubiomes)

endif()
