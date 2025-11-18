#pragma once

#include "terrain/biomes/biome.hpp"
#include "terrain/biomes/biome_t.hpp"
#include "terrain/generator.hpp"

class World;

/**
 * @class WorldGenLakes
 * @brief A final class responsible for generating lake structures in the world.
 * Inherits from AbstractWorldGenerator and provides methods for lake generation.
 */
class WorldGenLakes final : public AbstractWorldGenerator {
    Generator const *m_g; /**< Pointer to the generator used for terrain generation. */
    const lce::BlockState m_block; /**< The block state representing the type of lake (e.g., water or lava). */

public:
    /**
     * @brief Constructs a WorldGenLakes object with the specified generator and block state.
     * @param g Pointer to the generator used for terrain generation.
     * @param blockIn The block state representing the type of lake.
     */
    MU explicit WorldGenLakes(const Generator *g, const lce::BlockState blockIn)
        : m_g(g), m_block(blockIn) {
    }

    /**
     * @brief Generates a lake at the specified position in the world.
     * @param worldIn Pointer to the World object where the lake will be generated.
     * @param rng Reference to the random number generator.
     * @param pos The position where the lake should be generated.
     * @return True if the lake generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &pos) const override;
};

/**
 * @class FeaturePositions
 * @brief A utility class for generating feature positions such as lakes and dungeons in the world.
 */
class FeaturePositions {
public:
    /**
     * @brief Generates a water lake in the chunk.
     * @param g Pointer to the generator used for terrain generation.
     * @param chunkX The X-coordinate of the chunk.
     * @param chunkZ The Z-coordinate of the chunk.
     * @return The position of the water lake.
     */
    MU ND static Pos3D waterLake(const Generator *g, const int chunkX, const int chunkZ) {
        RNG rng = RNG::getPopulationSeed(g->getWorldSeed(), chunkX, chunkZ);
        return waterLake(g, rng, chunkX, chunkZ);
    }

    /**
     * @brief Generates a water lake in the chunk using the given random number generator.
     * @param g Pointer to the generator used for terrain generation.
     * @param rng Reference to the random number generator.
     * @param chunkX The X-coordinate of the chunk.
     * @param chunkZ The Z-coordinate of the chunk.
     * @return The position of the water lake.
     */
    ND static Pos3D waterLake(const Generator *g, RNG &rng, const int chunkX, const int chunkZ) {
        const biome_t biomeAt = g->getBiomeIdAt(1, (chunkX << 4) + 16, (chunkZ << 4) + 16);
        if (biomeAt == biome_t::desert || biomeAt == biome_t::desert_hills || rng.nextInt(4) != 0) return {};

        int xPos = (chunkX << 4) + rng.nextInt(16) + 8;
        int yPos = rng.nextInt(128);
        int zPos = (chunkZ << 4) + rng.nextInt(16) + 8;
        return {xPos, yPos, zPos};
    }

    /**
     * @brief Generates a lava lake in the chunk.
     * @param g Pointer to the generator used for terrain generation.
     * @param chunkX The X-coordinate of the chunk.
     * @param chunkZ The Z-coordinate of the chunk.
     * @return The position of the lava lake.
     */
    MU ND static Pos3D lavaLake(const Generator *g, const int chunkX, const int chunkZ) {
        RNG rng = RNG::getPopulationSeed(g->getWorldSeed(), chunkX, chunkZ);
        if (!waterLake(g, rng, chunkX, chunkZ).isNull()) return {};

        return lavaLake(rng, chunkX, chunkZ);
    }

    /**
     * @brief Generates a lava lake in the chunk using the given random number generator.
     * @param rng Reference to the random number generator.
     * @param chunkX The X-coordinate of the chunk.
     * @param chunkZ The Z-coordinate of the chunk.
     * @return The position of the lava lake.
     */
    ND static Pos3D lavaLake(RNG &rng, const int chunkX, const int chunkZ) {
        if (rng.nextInt(8) != 0) // no lava lake in chunk
            return {};

        int xPos = (chunkX << 4) + rng.nextInt(16) + 8;
        int yPos = rng.nextInt(rng.nextInt(120) + 8);
        int zPos = (chunkZ << 4) + rng.nextInt(16) + 8;
        if (yPos >= 63 && rng.nextInt(10) != 0) // no lava lake above sea level
            return {};

        return {xPos, yPos, zPos};
    }

    /**
     * @brief Generates a dungeon in the chunk.
     * @param g Pointer to the generator used for terrain generation.
     * @param chunkX The X-coordinate of the chunk.
     * @param chunkZ The Z-coordinate of the chunk.
     * @return The position of the dungeon.
     */
    MU ND static Pos3D dungeon(const Generator *g, const int chunkX, const int chunkZ) {
        RNG rng = RNG::getPopulationSeed(g->getWorldSeed(), chunkX, chunkZ);
        if (!waterLake(g, rng, chunkX, chunkZ).isNull()) return {};
        if (!lavaLake(rng, chunkX, chunkZ).isNull()) return {};

        return dungeon(rng, chunkX, chunkZ);
    }

    /**
     * @brief Generates a dungeon in the chunk using the given random number generator.
     * @param rng Reference to the random number generator.
     * @param chunkX The X-coordinate of the chunk.
     * @param chunkZ The Z-coordinate of the chunk.
     * @return The position of the dungeon.
     */
    ND static Pos3D dungeon(RNG &rng, const int chunkX, const int chunkZ) {
        int xPos = (chunkX << 4) + rng.nextInt(16) + 8;
        int yPos = rng.nextInt(128);
        int zPos = (chunkZ << 4) + rng.nextInt(16) + 8;
        return {xPos, yPos, zPos};
    }

    /**
     * @brief Advances the random number generator to skip unnecessary steps for decoration generation.
     * @param rng Reference to the random number generator.
     * @param chunkX The X-coordinate of the chunk.
     * @param chunkZ The Z-coordinate of the chunk.
     */
    MU static void decorations(RNG &rng, MU c_int chunkX, MU c_int chunkZ) {
        // generate ores
        // skip rng for now
        rng.advance<7950>();
    }
};
