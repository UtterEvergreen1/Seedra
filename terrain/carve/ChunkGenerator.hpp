#pragma once

#include <ctime>
#include <string>
#include <vector>

#include "NoiseGenerator.hpp"
#include "terrain/ChunkPrimer.hpp"

/**
 * @class ChunkGeneratorOverWorld
 * @brief Handles the generation of overworld chunks, including terrain, biomes, and heightmaps.
 */
class ChunkGeneratorOverWorld {
public:
    /**
     * @brief The generator used for biome and terrain generation.
     */
    const Generator *g;

    /**
     * @brief Random number generator used for chunk generation.
     */
    RNG rng{};

    /**
     * @brief Noise generator for minimum limit Perlin noise.
     */
    NoiseGeneratorOctaves minLimitPerlinNoise;

    /**
     * @brief Noise generator for maximum limit Perlin noise.
     */
    NoiseGeneratorOctaves maxLimitPerlinNoise;

    /**
     * @brief Noise generator for main Perlin noise.
     */
    NoiseGeneratorOctaves mainPerlinNoise;

    /**
     * @brief Noise generator for surface noise.
     */
    NoiseGeneratorPerlin surfaceNoise;

    /**
     * @brief Noise generator for scale noise (currently unused).
     */
    NoiseGeneratorOctaves scaleNoise;

    /**
     * @brief Noise generator for depth noise.
     */
    NoiseGeneratorOctaves depthNoise;

    /**
     * @brief Pointer to the array of biomes used for generation.
     */
    int *biomesForGeneration;

    std::span<int> biomesForGenerationSpan;

    /**
     * @brief Buffer for depth region values.
     */
    std::vector<double> depthRegion;

    /**
     * @brief Buffer for depth values.
     */
    std::vector<double> depthBuffer;

    /**
     * @brief Buffer for heightmap values.
     */
    std::vector<double> heightMap;

    /**
     * @brief Weights for biome generation.
     */
    std::vector<float> biomeWeights;

    /**
     * @brief Buffer for main noise region values.
     */
    std::vector<double> mainNoiseRegion;

    /**
     * @brief Buffer for minimum limit region values.
     */
    std::vector<double> minLimitRegion;

    /**
     * @brief Buffer for maximum limit region values.
     */
    std::vector<double> maxLimitRegion;

    /**
     * @brief Constructs a ChunkGeneratorOverWorld object with the specified generator.
     * @param generator The generator used for biome and terrain generation.
     */
    explicit ChunkGeneratorOverWorld(const Generator &generator);

    /**
     * @brief Destructor for the ChunkGeneratorOverWorld class.
     */
    ~ChunkGeneratorOverWorld();

    /**
     * @brief Sets the biomes for generation within a specified area.
     * @param x The starting X-coordinate.
     * @param z The starting Z-coordinate.
     * @param width The width of the area.
     * @param height The height of the area.
     * @param scale The scale of the biomes.
     */
    void setBiomesForGeneration(int x, int z, int width, int height, int scale);

    /**
     * @brief Sets the blocks in a chunk at the specified position.
     * @param chunkX The X-coordinate of the chunk.
     * @param chunkZ The Z-coordinate of the chunk.
     * @param primer Pointer to the ChunkPrimer to modify.
     */
    void setBlocksInChunk(int chunkX, int chunkZ, ChunkPrimer *primer);

    /**
     * @brief Replaces biome blocks in a chunk based on the biome data.
     * @param x The starting X-coordinate.
     * @param z The starting Z-coordinate.
     * @param primer Pointer to the ChunkPrimer to modify.
     * @param biomesIn Pointer to the array of biome IDs.
     */
    void replaceBiomeBlocks(int x, int z, ChunkPrimer *primer, c_int *biomesIn);

    /**
     * @brief Provides a chunk at the specified position.
     * @param chunkPrimer Pointer to the ChunkPrimer to populate.
     * @param x The X-coordinate of the chunk.
     * @param z The Z-coordinate of the chunk.
     */
    void provideChunk(ChunkPrimer *chunkPrimer, int x, int z);

    /**
     * @brief Provides a chunk at the specified position.
     * @param chunkPrimer Pointer to the ChunkPrimer to populate.
     * @param chunkPos The position of the chunk.
     */
    MU void provideChunk(ChunkPrimer *chunkPrimer, const Pos2D &chunkPos) {
        provideChunk(chunkPrimer, chunkPos.x, chunkPos.z);
    }

    /**
     * @brief Creates and provides a new chunk at the specified position.
     * @param x The X-coordinate of the chunk.
     * @param z The Z-coordinate of the chunk.
     * @return Pointer to the newly created ChunkPrimer.
     */
    ChunkPrimer *provideNewChunk(int x, int z) {
        ChunkPrimer *chunkPrimer = new ChunkPrimer();
        provideChunk(chunkPrimer, x, z);
        return chunkPrimer;
    }

    /**
     * @brief Creates and provides a new chunk at the specified position.
     * @param chunkPos The position of the chunk.
     * @return Pointer to the newly created ChunkPrimer.
     */
    MU ChunkPrimer *provideNewChunk(const Pos2D &chunkPos) { return provideNewChunk(chunkPos.x, chunkPos.z); }

    /**
     * @brief Generates the heightmap for a specified area.
     * @param x The starting X-coordinate.
     * @param y The starting Y-coordinate.
     * @param z The starting Z-coordinate.
     */
    void generateHeightmap(int x, int y, int z);
};
