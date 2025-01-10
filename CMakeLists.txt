cmake_minimum_required(VERSION 3.28.1)
project(GenChunks)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)


add_executable(GenChunks programs/gen_chunk.cpp)


target_include_directories(GenChunks PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


add_subdirectory(lce)
add_subdirectory(LegacyCubiomes)


target_link_libraries(GenChunks PRIVATE LegacyCubiomesLib)