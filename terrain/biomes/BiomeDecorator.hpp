#pragma once

#include "terrain/decorators/WorldGenBigMushroom.hpp"
#include "terrain/decorators/WorldGenBush.hpp"
#include "terrain/decorators/WorldGenCactus.hpp"
#include "terrain/decorators/WorldGenClay.hpp"
#include "terrain/decorators/WorldGenFlowers.hpp"
#include "terrain/decorators/WorldGenMinable.hpp"
#include "terrain/decorators/WorldGenReed.hpp"
#include "terrain/decorators/WorldGenSand.hpp"
#include "terrain/decorators/WorldGenWaterlily.hpp"

#include "lce/blocks/__include.hpp"
#include "common/Pos2DTemplate.hpp"
#include "common/Pos3DTemplate.hpp"
#include "common/rng.hpp"

/**
 * @class BiomeDecorator
 * @brief Handles the decoration of biomes by generating various features such as trees, flowers, ores, and more.
 */
class BiomeDecorator {
public:
    /**
     * @brief Virtual destructor for BiomeDecorator.
     */
    virtual ~BiomeDecorator();

protected:
    bool decorating = false; ///< Indicates if the decoration process is currently active.
    Pos3D chunkBlockPos; ///< The position of the chunk being decorated.

    /// The clay generator.
    WorldGenClay *clayGen = new WorldGenClay(4);

    /// The sand generator.
    WorldGenSand *sandGen = new WorldGenSand(lce::BlocksInit::SAND.getState(), 7);

    /// The gravel generator.
    WorldGenSand *gravelAsSandGen = new WorldGenSand(lce::BlocksInit::GRAVEL.getState(), 6);

    /// The dirt generator.
    WorldGenMinable *dirtGen{};
    /// The gravel generator.
    WorldGenMinable *gravelGen{};
    /// The granite generator.
    WorldGenMinable *graniteGen{};
    /// The diorite generator.
    WorldGenMinable *dioriteGen{};
    /// The andesite generator.
    WorldGenMinable *andesiteGen{};
    /// The coal ore generator.
    WorldGenMinable *coalGen{};
    /// The iron ore generator.
    WorldGenMinable *ironGen{};

    /// The gold ore generator.
    WorldGenMinable *goldGen{};
    /// The redstone ore generator.
    WorldGenMinable *redstoneGen{};
    /// The diamond ore generator.
    WorldGenMinable *diamondGen{};

    /// The lapis ore generator.
    WorldGenMinable *lapisGen{};

    /// The flower generator for dandelions.
    WorldGenFlowers *flowerGen = new WorldGenFlowers(lce::blocks::DANDELION_ID, BlockFlower::EnumFlowerType::DANDELION);

    /// The brown mushroom generator.
    WorldGenBush *mushroomBrownGen = new WorldGenBush(lce::BlocksInit::BROWN_MUSHROOM.getState());

    /// The red mushroom generator.
    WorldGenBush *mushroomRedGen = new WorldGenBush(lce::BlocksInit::RED_MUSHROOM.getState());

    /// The big mushroom generator.
    WorldGenBigMushroom *bigMushroomGen = new WorldGenBigMushroom();

    /// The reed generator.
    WorldGenReed *reedGen = new WorldGenReed();

    /// The cactus generator.
    WorldGenCactus *cactusGen = new WorldGenCactus();

    /// The water lily generator.
    WorldGenWaterlily *waterlilyGen = new WorldGenWaterlily();

public:
    int treesPerChunk = 0; ///< Number of trees generated per chunk.
    int flowersPerChunk = 2; ///< Number of flowers generated per chunk.
    float extraTreeChance = 0.1F; ///< Chance for generating extra trees.
    int grassPerChunk = 1; ///< Number of grass patches generated per chunk.
    int deadBushPerChunk = 0; ///< Number of dead bushes generated per chunk.
    int mushroomsPerChunk = 0; ///< Number of mushrooms generated per chunk.
    int reedsPerChunk = 0; ///< Number of reeds generated per chunk.
    int cactiPerChunk = 0; ///< Number of cacti generated per chunk.
    int sandPatchesPerChunk = 3; ///< Number of sand patches generated per chunk.
    int gravelPatchesPerChunk = 1; ///< Number of gravel patches generated per chunk.
    int clayPerChunk = 1; ///< Number of clay patches generated per chunk.
    int bigMushroomsPerChunk = 0; ///< Number of big mushrooms generated per chunk.
    int waterlilyPerChunk = 0; ///< Number of water lilies generated per chunk.

protected:
    /**
     * @brief Generates decorations for the biome.
     * @param world Pointer to the world.
     * @param biome Pointer to the biome being decorated.
     * @param rng Random number generator.
     */
    void genDecorations(World *world, Biome *biome, RNG &rng);

    /**
     * @brief Generates ores for the biome.
     * @param world Pointer to the world.
     * @param rng Random number generator.
     */
    virtual void generateOres(World *world, RNG &rng);

    /**
     * @brief Generates standard ores using the first method.
     * @param world Pointer to the world.
     * @param rng Random number generator.
     * @param gen Pointer to the ore generator.
     * @param count Number of ores to generate.
     * @param minHeight Minimum height for ore generation.
     * @param maxHeight Maximum height for ore generation.
     */
    void genStandardOre1(World *world, RNG &rng, const WorldGenMinable *gen, int count, int minHeight,
                         int maxHeight) const;

    /**
     * @brief Generates standard ores using the second method.
     * @param world Pointer to the world.
     * @param rng Random number generator.
     * @param gen Pointer to the ore generator.
     * @param count Number of ores to generate.
     * @param centerHeight Center height for ore generation.
     * @param spread Spread for ore generation.
     */
    void genStandardOre2(World *world, RNG &rng, const WorldGenMinable *gen, int count, int centerHeight,
                         int spread) const;

public:
    /**
     * @brief Decorates the biome at a specific position.
     * @param world Pointer to the world.
     * @param biome Pointer to the biome being decorated.
     * @param rng Random number generator.
     * @param posX X-coordinate of the position.
     * @param posZ Z-coordinate of the position.
     */
    void decorate(World *world, Biome *biome, RNG &rng, int posX, int posZ);

    /**
     * @brief Decorates the biome at a specific position.
     * @param world Pointer to the world.
     * @param biome Pointer to the biome being decorated.
     * @param rng Random number generator.
     * @param pos Position in the world.
     */
    void decorate(World *world, Biome *biome, RNG &rng, const Pos2D &pos);
};
