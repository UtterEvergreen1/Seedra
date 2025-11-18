#pragma once

#include <ctime>
#include <string>
#include <vector>
#include <chrono>

#include "terrain/ChunkPrimer.hpp"
#include "terrain/noise/NoiseGen.hpp"


class WorldNoise;
extern double timeNoiseSetup;


/**
 * @class ChunkGeneratorOverWorld
 * @brief Handles the generation of overworld chunks, including terrain, biomes, and heightmaps.
 */
class ChunkGeneratorOverWorld {
    static constexpr int HEIGHT_FALLOFF_BORDER = 32;
    static constexpr int HEIGHT_FALLOFF_BORDER_CHUNKS = (HEIGHT_FALLOFF_BORDER + 15) / 16; // in chunk coordinates (rounded up)
public:

    const Generator *g; ///< The generator used for biome and terrain generation.

    const ChunkNoise& chunkNoise; ///< reference to the world noise from generator.

    RNG rng{}; ///< Random number generator used for chunk generation.

    biome_t *biomesForGeneration; ///< Pointer to the array of biomes used for generation.

    std::span<int> biomesForGenerationSpan;

    std::array<double, 25> depthRegion;     ///< Buffer for depth region values.
    std::array<double, 256> depthBuffer;     ///< Buffer for depth values.
    std::array<double, 825> heightMap;       ///< Buffer for heightmap values.
    std::array<double, 825> mainNoiseRegion; ///< Buffer for main noise region values.
    std::array<double, 825> minLimitRegion;  ///< Buffer for minimum limit region values.
    std::array<double, 825> maxLimitRegion;  ///< Buffer for maximum limit region values.

    /**
     * @brief Constructs a ChunkGeneratorOverWorld object with the specified generator.
     * @param generator The generator used for biome and terrain generation.
     */
    explicit ChunkGeneratorOverWorld(const Generator &generator);

    ~ChunkGeneratorOverWorld(); ///< @brief Destructor for the ChunkGeneratorOverWorld class.


    /*
    void generateHeightmap3(int x, int y, int z) {
        auto t0 = std::chrono::high_resolution_clock::now();

        depthNoise.genNoiseOctaves(g, depthRegion, x, 10, z, 5, 1, 5, 200.0, 1.0, 200.0);
        auto t1 = std::chrono::high_resolution_clock::now();

        mainPerlinNoise.genNoiseOctaves(g, mainNoiseRegion, x, y, z, 5, 33, 5, 8.55515, 4.277575, 8.55515);
        auto t2 = std::chrono::high_resolution_clock::now();

        minLimitPerlinNoise.genNoiseOctaves(g, minLimitRegion, x, y, z, 5, 33, 5, 684.412, 684.412, 684.412);
        auto t3 = std::chrono::high_resolution_clock::now();

        maxLimitPerlinNoise.genNoiseOctaves(g, maxLimitRegion, x, y, z, 5, 33, 5, 684.412, 684.412, 684.412);
        auto t4 = std::chrono::high_resolution_clock::now();

        printf("Depth:  %lld ns\n", (t1-t0).count());
        printf("Main:   %lld ns\n", (t2-t1).count());
        printf("MinLim: %lld ns\n", (t3-t2).count());
        printf("MaxLim: %lld ns\n", (t4-t3).count());
    }
     */

    /*
    void genNoiseOctavesPaired(int x, int y, int z) {
        constexpr int xSize = 5, ySize = 33, zSize = 5;
        constexpr double xScale = 684.412, yScale = 684.412, zScale = 684.412;
        constexpr int totalSize = xSize * ySize * zSize;

        // Clear arrays
        std::memset(minLimitRegion.data(), 0, totalSize * sizeof(double));
        std::memset(maxLimitRegion.data(), 0, totalSize * sizeof(double));

        double amplitude = 1.0;

        for (int octave = 0; octave < 16; ++octave) {
            // Compute scaled offsets
            const double sx = x * amplitude * xScale;
            const double sy = y * amplitude * yScale;
            const double sz = z * amplitude * zScale;

            // Wrap coordinates (copied from genNoiseOctaves)
            i64 flooredX = lfloor(sx);
            i64 flooredZ = lfloor(sz);

            double xFrac = sx - static_cast<double>(flooredX);
            double zFrac = sz - static_cast<double>(flooredZ);

            flooredX %= 16777216LL;
            flooredZ %= 16777216LL;

            xFrac += static_cast<double>(flooredX);
            zFrac += static_cast<double>(flooredZ);

            const double scale_x = xScale * amplitude;
            const double scale_y = yScale * amplitude;
            const double scale_z = zScale * amplitude;

            // Compute min and max in sequence (permutation tables stay hot!)
            minLimitPerlinNoise.generatorCollection[octave].populateNoiseArrayNew(
                g, minLimitRegion,
                xFrac, sy, zFrac,
                xSize, ySize, zSize,
                scale_x, scale_y, scale_z,
                amplitude);

            maxLimitPerlinNoise.generatorCollection[octave].populateNoiseArrayNew(
                g, maxLimitRegion,
                xFrac, sy, zFrac,
                xSize, ySize, zSize,
                scale_x, scale_y, scale_z,
                amplitude);

            amplitude *= 0.5;
        }
    }
    */



    /**
     * @brief Sets the biomes for generation within a specified area.
     * @param x The starting X-coordinate.
     * @param z The starting Z-coordinate.
     * @param width The width of the area.
     * @param height The height of the area.
     * @param scale The scale of the biomes.
     */
    void setBiomesForGeneration(int x, int z, int width, int height, uint32_t scale);

    /**
     * @brief Sets the blocks in a chunk at the specified position.
     * @param chunkX The X-coordinate of the chunk.
     * @param chunkZ The Z-coordinate of the chunk.
     * @param primer Pointer to the ChunkPrimer to modify.
     */
    void setBlocksInChunk(int chunkX, int chunkZ, ChunkPrimer *primer);

    /**
     * @brief Replaces biome blocks in a chunk based on the biome data.
     * @param chunkX The starting X-coordinate.
     * @param chunkZ The starting Z-coordinate.
     * @param primer Pointer to the ChunkPrimer to modify.
     */
    void replaceBiomeBlocks(int chunkX, int chunkZ, ChunkPrimer *primer);

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
        auto *chunkPrimer = new ChunkPrimer();
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

    bool hasHeightFalloff(int chunkX, int chunkZ) const;

    double getHeightFalloff(int blockX, int blockZ) const;

};
