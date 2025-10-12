cmake_minimum_required(VERSION 3.24)
project(LIB_LegacyCubiomes)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    #  /O2  = maximise speed   /GL = link-time-code-gen   /Zi = keep PDB in Debug
    set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE} /O2 /GL")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /GL")
    set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG}  /Od /Zi")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi")
    set(LEGACY_CUBIOMES_COMPILE_OPTIONS /arch:AVX /MP)
else()
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -static-libgcc -static-libstdc++")
    set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE}   -O3 -s")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -s")
    set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG}   -O0")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -O0")
    set(LEGACY_CUBIOMES_COMPILE_OPTIONS "-mavx2;-msse4.1")
endif()




# Build lceLib from the submodule
add_subdirectory(lce)

# Gather all the .cpp/.hpp for LegacyCubiomes
file(GLOB_RECURSE LEGACY_CUBIOMES_SOURCES
        # submodule
        "${CMAKE_CURRENT_LIST_DIR}/lce/*.*pp"
        # project
        "${CMAKE_CURRENT_LIST_DIR}/common/*.*pp"
        "${CMAKE_CURRENT_LIST_DIR}/components/*.*pp"
        "${CMAKE_CURRENT_LIST_DIR}/chunk_detector/*.*pp"
        "${CMAKE_CURRENT_LIST_DIR}/terrain/*.*pp"
        "${CMAKE_CURRENT_LIST_DIR}/cubiomes/*.*pp"
        "${CMAKE_CURRENT_LIST_DIR}/enchants/*.*pp"
        "${CMAKE_CURRENT_LIST_DIR}/include/*.*pp"
        "${CMAKE_CURRENT_LIST_DIR}/loot/*.*pp"
        "${CMAKE_CURRENT_LIST_DIR}/structures/*.*pp"
        ...
)

add_library(LIB_LegacyCubiomes STATIC ${LEGACY_CUBIOMES_SOURCES})
target_compile_features(LIB_LegacyCubiomes PRIVATE cxx_std_20)
target_compile_options(LIB_LegacyCubiomes PRIVATE ${LEGACY_CUBIOMES_COMPILE_OPTIONS})


# Link the lce library
# target_link_libraries(LIB_LegacyCubiomes PRIVATE LIB_lce)

# If your code does #include "LegacyCubiomes/xyz.hpp",
# you need the parent of this folder on the path:
target_include_directories(LIB_LegacyCubiomes
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
)

set(ASSET_DEST "/assets/lce")


function(add_cli NAME SRC)
    add_executable(${NAME} "${SRC}")
    target_link_libraries(${NAME} PRIVATE LIB_LegacyCubiomes)

    target_include_directories(${NAME} PRIVATE ${LCE_INCLUDE_DIR})
    target_compile_options(${NAME} PRIVATE ${LCE_COMPILE_OPTIONS} ${LEGACY_CUBIOMES_COMPILE_OPTIONS})
    target_compile_definitions(${NAME} PRIVATE ${LCE_COMPILE_DEFINITIONS})


    # add_custom_command(TARGET ${NAME} POST_BUILD
    #         COMMAND ${CMAKE_COMMAND} -E make_directory
    #         "${LCE_ASSETS_DEST_DIR}"
    #         COMMAND  ${CMAKE_COMMAND} -E copy_directory
    #         ${LCE_ASSETS_SOURCE_DIR} ${ASSET_DEST}
    #         COMMENT "Copying lce assets → ${LCE_ASSETS_DEST_DIR}"
    # )


endfunction()




# (Optional) If you want a standalone LegacyCubiomesExe for testing:
unset(LC_BUILD_EXECUTABLE CACHE)
option(LC_BUILD_EXECUTABLE "Build LegacyCubiomes standalone executable" ON)
if(LC_BUILD_EXECUTABLE)

    add_cli(EXE_GenCreateWorld "tests/gen_chunk.cpp")
    add_cli(EXE_DrawImages "tests/draw_images.cpp")
    add_cli(EXE_GetMaxTempBiomes "tests/get_min_temp_biomes.cpp")
    add_cli(EXE_MineAChunkSF "tests/best_mine_chunk_seed.cpp")
    add_cli(EXE_MineAChunkSF2 "tests/best_mine_chunk_seed2.cpp")
    add_cli(EXE_TestVillageGen "tests/test_village_gen_speed.cpp")
    add_cli(EXE_TestLootEnchants "tests/test_loot_enchants.cpp")
    add_cli(EXE_TestFindVillages "tests/find_villages.cpp")
    add_cli(EXE_TestReverser "tests/test_reverser.cpp")
    add_cli(EXE_HighCake "tests/find_highest_cake.cpp")
    add_cli(EXE_HighObi "tests/high_obi_blacksmith.cpp")
    add_cli(EXE_CreateHeatMap "tests/create_heat_map.cpp")
    add_cli(EXE_TallCacti "tests/find_tall_cactus.cpp")
    add_cli(EXE_AllBiomes "tests/all_biomes_and_structures.cpp")
    add_cli(EXE_11Eye "tests/11_eye_filter.cpp")

endif()
