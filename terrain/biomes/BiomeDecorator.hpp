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


class Biome;
class ChunkPrimer;
class WorldGenMinable;

class BiomeDecorator {
public:
    virtual ~BiomeDecorator();

protected:
    bool decorating = false;
    Pos3D chunkBlockPos;
    /// The clay generator.
    WorldGenClay *clayGen = new WorldGenClay(4);

    /// The sand generator.
    WorldGenSand *sandGen = new WorldGenSand(&lce::BlocksInit::SAND, 7);

    /// The gravel generator.
    WorldGenSand *gravelAsSandGen = new WorldGenSand(&lce::BlocksInit::GRAVEL, 6);

    /// The dirt generator.
    WorldGenMinable *dirtGen{};
    WorldGenMinable *gravelGen{};
    WorldGenMinable *graniteGen{};
    WorldGenMinable *dioriteGen{};
    WorldGenMinable *andesiteGen{};
    WorldGenMinable *coalGen{};
    WorldGenMinable *ironGen{};

    /// Field that holds gold WorldGenMinable
    WorldGenMinable *goldGen{};
    WorldGenMinable *redstoneGen{};
    WorldGenMinable *diamondGen{};

    /// Field that holds Lapis WorldGenMinable
    WorldGenMinable *lapisGen{};

    WorldGenFlowers* flowerGen = new WorldGenFlowers(lce::blocks::DANDELION_ID, BlockFlower::EnumFlowerType::DANDELION);

    /// Field that holds mushroomBrown WorldGenFlowers
    WorldGenBush* mushroomBrownGen = new WorldGenBush(&lce::BlocksInit::BROWN_MUSHROOM);

    /// Field that holds mushroomRed WorldGenFlowers
    WorldGenBush* mushroomRedGen = new WorldGenBush(&lce::BlocksInit::RED_MUSHROOM);

    /// Field that holds big mushroom generator
    WorldGenBigMushroom* bigMushroomGen = new WorldGenBigMushroom();

    /// Field that holds WorldGenReed
    WorldGenReed* reedGen = new WorldGenReed();

    /// Field that holds WorldGenCactus
    WorldGenCactus* cactusGen = new WorldGenCactus();

    /// The water lily generation!
    WorldGenWaterlily* waterlilyGen = new WorldGenWaterlily();

public:
    int treesPerChunk = 0;
    int flowersPerChunk = 2;
    float extraTreeChance = 0.1F;
    int grassPerChunk = 1;
    int deadBushPerChunk = 0;
    int mushroomsPerChunk = 0;
    int reedsPerChunk = 0;
    int cactiPerChunk = 0;
    int sandPatchesPerChunk = 3;
    int gravelPatchesPerChunk = 1;
    int clayPerChunk = 1;
    int bigMushroomsPerChunk = 0;
    int waterlilyPerChunk = 0;

protected:
    void genDecorations(World *world, Biome* biome, RNG& rng);
    virtual void generateOres(World *world, RNG& rng);
    void genStandardOre1(World *world, RNG& rng, const WorldGenMinable* gen, int count, int minHeight, int maxHeight) const;
    void genStandardOre2(World *world, RNG &rng, const WorldGenMinable *gen, int count, int centerHeight, int spread) const;

public:
    void decorate(World *world, Biome* biome, RNG& rng, int posX, int posZ);
    void decorate(World *world, Biome* biome, RNG& rng, const Pos2D &pos);
};

