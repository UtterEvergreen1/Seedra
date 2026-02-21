cmake_minimum_required(VERSION 3.23.1)
project(LIB_Seedra)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    # ----------------------------
    # Windows / MSVC
    # ----------------------------
    # DEBUG
    set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG}  /Od /Zi")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi")

    # RELEASE
    set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE} /O2 /GL")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /GL")

    # RELWITHDEBINFO
    set(CMAKE_C_FLAGS_RELWITHDEBINFO   "${CMAKE_C_FLAGS_RELWITHDEBINFO} /O2 /Zi /Zo /Oy- /FC")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /O2 /Zi /Zo /Oy- /FC")
    add_link_options($<$<CONFIG:RelWithDebInfo>:/DEBUG:FULL>)

    # PDB
    set(CMAKE_PDB_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/pdb")
    set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/pdb")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/pdb")

    set(SEEDRA_COMPILE_OPTIONS /arch:AVX /MP)

elseif(APPLE)
    # ----------------------------
    # macOS / Apple Clang
    # ----------------------------
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -Wl,-dead_strip")  # optional

    set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE}   -O3")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

    # set(CMAKE_C_FLAGS_DEBUG
    #         "${CMAKE_C_FLAGS_DEBUG} -O0 -g -Wall -Wextra -Wpedantic -Wshadow -Wcast-align -Wconversion -Wsign-conversion -Wno-global-constructors -Wold-style-cast -Wundef -Wunreachable-code -Wno-covered-switch-default -Wfloat-equal -Wpointer-arith -Wstrict-prototypes -Wredundant-decls -Weverything"
    # )
    # set(CMAKE_CXX_FLAGS_DEBUG
    #         "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -Wall -Wextra -Wpedantic -Wshadow -Wcast-align -Wconversion -Wsign-conversion -Wno-global-constructors -Wold-style-cast -Wundef -Wunreachable-code -Wno-covered-switch-default -Wfloat-equal -Wpointer-arith -Wstrict-prototypes -Wredundant-decls -Weverything"
    # )

    set(CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -g -fno-omit-frame-pointer -fno-optimize-sibling-calls")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -fno-omit-frame-pointer -fno-optimize-sibling-calls")

    # Apple Clang being weird
    add_compile_options(-Wno-c++98-compat -Wno-c++98-compat-pedantic)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++23")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++23")

    if(EMSCRIPTEN)
        # WASM
        set(SEEDRA_COMPILE_OPTIONS "")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
        # Intel Macs: enable AVX/SSE
        set(SEEDRA_COMPILE_OPTIONS "-mavx2;-msse4.1")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
        # Apple Silicon: NEON instructions used automatically; no x86 flags
        set(SEEDRA_COMPILE_OPTIONS "")
    endif()

else()
    # ----------------------------
    # Linux / other UNIX
    # ----------------------------
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -static-libgcc -static-libstdc++")

    set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE}   -O3 -s")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -s")

    set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS_DEBUG}   -O0")
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -O0")

    set(CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -g -fno-omit-frame-pointer -fno-optimize-sibling-calls")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -fno-omit-frame-pointer -fno-optimize-sibling-calls")

    if(EMSCRIPTEN)
        set(SEEDRA_COMPILE_OPTIONS "")
    else()
        set(SEEDRA_COMPILE_OPTIONS "-mavx2;-msse4.1")
    endif()

endif()




# Build lceLib from the submodule
add_subdirectory(lce)

# Gather all the .cpp/.hpp for Seedra
file(GLOB_RECURSE SEEDRA_SOURCES
        # submodule
        "${CMAKE_CURRENT_LIST_DIR}/lce/*.*pp"
        # project
        "${CMAKE_CURRENT_LIST_DIR}/common/*.*pp"
        "${CMAKE_CURRENT_LIST_DIR}/components/*.*pp"
        "${CMAKE_CURRENT_LIST_DIR}/terrain/*.*pp"
        "${CMAKE_CURRENT_LIST_DIR}/enchants/*.*pp"
        "${CMAKE_CURRENT_LIST_DIR}/include/*.*pp"
        "${CMAKE_CURRENT_LIST_DIR}/loot/*.*pp"
        "${CMAKE_CURRENT_LIST_DIR}/structures/*.*pp"
        ...
)

add_library(LIB_Seedra STATIC ${SEEDRA_SOURCES})
target_compile_features(LIB_Seedra PRIVATE cxx_std_23)
target_compile_options(LIB_Seedra PRIVATE ${SEEDRA_COMPILE_OPTIONS} -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-c++20-compat -Wno-c++20-compat-pedantic -Wno-exit-time-destructors -Wno-float-equal -Wno-padded -Wno-switch-default)


# Link the lce library
# target_link_libraries(LIB_Seedra PRIVATE LIB_lce)

# If your code does #include "Seedra/xyz.hpp",
# you need the parent of this folder on the path:
target_include_directories(LIB_Seedra
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
)
set_target_properties(LIB_Seedra PROPERTIES
        COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY_RELWITHDEBINFO}"
)

set(ASSET_DEST "/assets/lce")


function(add_cli NAME SRC)
    add_executable(${NAME} "${SRC}")
    target_link_libraries(${NAME} PRIVATE LIB_Seedra)


    target_include_directories(${NAME} PRIVATE ${LCE_INCLUDE_DIR})
    target_compile_options(${NAME} PRIVATE ${LCE_COMPILE_OPTIONS} ${SEEDRA_COMPILE_OPTIONS} -fexceptions -Wno-c++98-compat -Wno-c++98-compat-pedantic)
    target_compile_definitions(${NAME} PRIVATE ${LCE_COMPILE_DEFINITIONS})
    set_target_properties(${NAME} PROPERTIES
            PDB_OUTPUT_DIRECTORY "${CMAKE_PDB_OUTPUT_DIRECTORY_RELWITHDEBINFO}"
            COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY_RELWITHDEBINFO}"
    )


    # add_custom_command(TARGET ${NAME} POST_BUILD
    #         COMMAND ${CMAKE_COMMAND} -E make_directory
    #         "${LCE_ASSETS_DEST_DIR}"
    #         COMMAND  ${CMAKE_COMMAND} -E copy_directory
    #         ${LCE_ASSETS_SOURCE_DIR} ${ASSET_DEST}
    #         COMMENT "Copying lce assets → ${LCE_ASSETS_DEST_DIR}"
    # )


endfunction()




# (Optional) If you want a standalone Seedra exe for testing:

if(NOT DEFINED LC_BUILD_EXECUTABLE)
    if(EMSCRIPTEN)
        set(LC_BUILD_EXECUTABLE OFF CACHE BOOL "Build Seedra standalone executables" FORCE)
    else()
        option(LC_BUILD_EXECUTABLE "Build Seedra standalone executables" ON)
    endif()
endif()

if(LC_BUILD_EXECUTABLE)

    add_cli(EXE_GenCreateWorld "tests/gen_chunk.cpp")
    add_cli(EXE_DrawBiomes "tests/draw_biomes.cpp")
    add_cli(EXE_GetMaxTempBiomes "tests/get_min_temp_biomes.cpp")
    add_cli(EXE_MineAChunkSF "tests/best_mine_chunk_seed.cpp")
    add_cli(EXE_MineAChunkSF2 "tests/best_mine_chunk_seed2.cpp")
    add_cli(EXE_MineAChunkSF3 "tests/best_mine_chunk_seed3.cpp")
    add_cli(EXE_MineAChunkSF4 "tests/best_mine_chunk_seed4.cpp")
    add_cli(EXE_MineAChunkSF4P2 "tests/best_mine_chunk_seed4_pass2.cpp")
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
    add_cli(EXE_StructureSanity "tests/structure_sanity.cpp")
    add_cli(EXE_GetBiomeDepthAndScaleOptimizer "tests/getBiomeDepthAndScaleOptimizer.cpp")
    add_cli(EXE_FindSlowGenCode "tests/find_gen_slow_code.cpp")
    add_cli(EXE_NoisePopulateParityTest "tests/NoisePopulateParityTest.cpp")
    add_cli(EXE_NoiseProfiler "tests/noiseProfiler.cpp")
    add_cli(EXE_DrawSurfaceNoise "tests/surface_noise_visualizer.cpp")
    add_cli(EXE_StructureReader "tests/structureReader.cpp")
endif()
