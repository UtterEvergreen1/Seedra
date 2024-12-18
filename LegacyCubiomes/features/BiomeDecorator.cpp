#include "BiomeDecorator.hpp"
#include "LegacyCubiomes/chunk_generator/biome.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"
#include "WorldGenerator/WorldGenCucurbitsBlock.hpp"
#include "WorldGenerator/WorldGenDeadBush.hpp"
#include "WorldGenerator/WorldGenLiquids.hpp"
#include "WorldGenerator/WorldGenMinable.hpp"

#define STONE_SUBS_SIZE 33
#define COAL_SIZE 17
#define IRON_SIZE 9
#define GOLD_SIZE 9
#define REDSTONE_SIZE 8
#define DIAMOND_SIZE 8
#define LAPIS_SIZE 7

#define STONE_SUBS_COUNT 10
#define COMMON_ORES_COUNT 20
#define UNCOMMON_ORES_COUNT 8
#define RARE_ORES_COUNT 2
#define VERY_RARE_ORES_COUNT 1

#define ORES_MIN_HEIGHT 0
#define HIGH_ORES_MAX_HEIGHT 128
#define STONE_SUBS_MAX_HEIGHT 80
#define IRON_ORE_MAX_HEIGHT 64
#define GOLD_ORE_MAX_HEIGHT 32
#define DIAMOND_ORE_MAX_HEIGHT 16
#define LAPIS_CENTER_HEIGHT 16
#define LAPIS_SPREAD 16

BiomeDecorator::~BiomeDecorator() {
    delete this->clayGen;
    delete this->sandGen;
    delete this->gravelAsSandGen;
    delete this->dirtGen;
    delete this->gravelGen;
    delete this->graniteGen;
    delete this->dioriteGen;
    delete this->andesiteGen;
    delete this->coalGen;
    delete this->ironGen;
    delete this->goldGen;
    delete this->redstoneGen;
    delete this->diamondGen;
    delete this->lapisGen;
    delete this->flowerGen;
    delete this->mushroomBrownGen;
    delete this->mushroomRedGen;
    delete this->bigMushroomGen;
    delete this->reedGen;
    delete this->cactusGen;
    delete this->waterlilyGen;
}

void BiomeDecorator::decorate(ChunkPrimer *chunk, Biome *biome, RNG &rng, int chunkX, int chunkZ) {
    if (this->decorating) {
        return;
    }

    this->decorating = true;
    this->chunkPos = {chunkX, 0, chunkZ};
    this->dirtGen = new WorldGenMinable(&lce::blocks::DIRT, STONE_SUBS_SIZE);
    this->gravelGen = new WorldGenMinable(&lce::blocks::GRAVEL, STONE_SUBS_SIZE);
    this->graniteGen = new WorldGenMinable(&lce::blocks::GRANITE, STONE_SUBS_SIZE);
    this->dioriteGen = new WorldGenMinable(&lce::blocks::DIORITE, STONE_SUBS_SIZE);
    this->andesiteGen = new WorldGenMinable(&lce::blocks::ANDESITE, STONE_SUBS_SIZE);
    this->coalGen = new WorldGenMinable(&lce::blocks::COAL_ORE, COAL_SIZE);
    this->ironGen = new WorldGenMinable(&lce::blocks::IRON_ORE, IRON_SIZE);
    this->goldGen = new WorldGenMinable(&lce::blocks::GOLD_ORE, GOLD_SIZE);
    this->redstoneGen = new WorldGenMinable(&lce::blocks::REDSTONE_ORE, REDSTONE_SIZE);
    this->diamondGen = new WorldGenMinable(&lce::blocks::DIAMOND_ORE, DIAMOND_SIZE);
    this->lapisGen = new WorldGenMinable(&lce::blocks::LAPIS_LAZULI_ORE, LAPIS_SIZE);
    this->genDecorations(chunk, biome, rng);
    this->decorating = false;
}

void BiomeDecorator::decorate(ChunkPrimer *chunk, Biome *biome, RNG &rng, const Pos2D &chunkPos) {
    this->decorate(chunk, biome, rng, chunkPos.x, chunkPos.z);
}

void BiomeDecorator::genDecorations(ChunkPrimer *chunk, Biome *biome, RNG &rng) {
    this->generateOres(chunk, rng);

    for (int i = 0; i < this->sandPatchesPerChunk; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        this->sandGen->generate(chunk, rng, this->chunkPos.add(x, 0, z));
    }

    for (int i = 0; i < this->clayPerChunk; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        this->sandGen->generate(chunk, rng, this->chunkPos.add(x, 0, z));
    }

    for (int i = 0; i < this->gravelPatchesPerChunk; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        this->gravelAsSandGen->generate(chunk, rng, this->chunkPos.add(x, 0, z));
    }

    int trees = this->treesPerChunk;
    if (rng.nextFloat() < this->extraTreeChance) {
        ++trees;
    }
    for (int i = 0; i < trees; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        auto *tree = biome->genBigTreeChance(rng);
        tree->setDecorationDefaults();
        Pos3D blockPos = chunk->getHeightPos(this->chunkPos.add(x, 0, z));
        if (tree->generate(chunk, rng, blockPos)) {
            tree->generateSaplings(chunk, rng, blockPos);
        }
    }

    for (int i = 0; i < this->bigMushroomsPerChunk; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        this->bigMushroomGen->generate(chunk, rng, chunk->getHeightPos(this->chunkPos.add(x, 0, z)));
    }

    for (int i = 0; i < this->flowersPerChunk; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(chunk->getHeight(this->chunkPos.add(x, 0, z)) + 32);

        Pos3D blockPos = this->chunkPos.add(x, y, z);
        BlockFlower::EnumFlowerType flowerType = biome->pickRandomFlower(rng, blockPos.convert2D());

        c_int flowerBlock = flowerType == BlockFlower::EnumFlowerType::DANDELION ? lce::blocks::ids::DANDELION_ID : lce::blocks::ids::POPPY_ID;
        this->flowerGen->setGeneratedBlock(flowerBlock, flowerType);
        this->flowerGen->generate(chunk, rng, blockPos);
    }

    for (int i = 0; i < this->grassPerChunk; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(chunk->getHeight(this->chunkPos.add(x, 0, z)) * 2);
        biome->getRandomWorldGenForGrass(rng)->generate(chunk, rng, this->chunkPos.add(x, y, z));
    }

    //seagrass here

    for (int i = 0; i < this->deadBushPerChunk; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(chunk->getHeight(this->chunkPos.add(x, 0, z)) * 2);
        (WorldGenDeadBush()).generate(chunk, rng, this->chunkPos.add(x, y, z));
    }

    for (int i = 0; i < this->waterlilyPerChunk; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(chunk->getHeight(this->chunkPos.add(x, 0, z)) * 2);

        Pos3D blockPos = this->chunkPos.add(x, y, z);
        while (blockPos.getY() > 0) {
            blockPos = blockPos.down();
            if (!chunk->isAirBlock(blockPos)) {
                break;
            }
        }

        this->waterlilyGen->generate(chunk, rng, blockPos.up());
    }

    for (int i = 0; i < this->mushroomsPerChunk; ++i) {
        if (rng.nextInt(4) == 0) {
            c_int x = rng.nextInt(16) + 8;
            c_int z = rng.nextInt(16) + 8;
            this->mushroomBrownGen->generate(chunk, rng, chunk->getHeightPos(this->chunkPos.add(x, 0, z)));
        }

        if (rng.nextInt(8) == 0) {
            c_int x = rng.nextInt(16) + 8;
            c_int z = rng.nextInt(16) + 8;
            c_int y = rng.nextInt(chunk->getHeight(this->chunkPos.add(x, 0, z)) * 2);
            this->mushroomRedGen->generate(chunk, rng, this->chunkPos.add(x, y, z));
        }
    }

    if (rng.nextInt(4) == 0) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(chunk->getHeight(this->chunkPos.add(x, 0, z)) * 2);
        this->mushroomBrownGen->generate(chunk, rng, this->chunkPos.add(x, y, z));
    }

    if (rng.nextInt(8) == 0) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(chunk->getHeight(this->chunkPos.add(x, 0, z)) * 2);
        this->mushroomRedGen->generate(chunk, rng, this->chunkPos.add(x, y, z));
    }

    //kelp here

    for (int i = 0; i < this->reedsPerChunk + 10; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(chunk->getHeight(this->chunkPos.add(x, 0, z)) * 2);
        this->reedGen->generate(chunk, rng, this->chunkPos.add(x, y, z));
    }

    if (rng.nextInt(32) == 0) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(chunk->getHeight(this->chunkPos.add(x, 0, z)) * 2);
        (WorldGenCucurbitsBlock(lce::blocks::ids::PUMPKIN_ID, true)).generate(chunk, rng, this->chunkPos.add(x, y, z));
    }

    for (int i = 0; i < this->cactiPerChunk; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(chunk->getHeight(this->chunkPos.add(x, 0, z)) * 2);
        this->cactusGen->generate(chunk, rng, this->chunkPos.add(x, y, z));
    }

    for (int i = 0; i < 50; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(rng.nextInt(120) + 8);
        (WorldGenLiquids(lce::blocks::ids::FLOWING_WATER_ID)).generate(chunk, rng, this->chunkPos.add(x, y, z));
    }

    for (int i = 0; i < 20; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(rng.nextInt(rng.nextInt(112) + 8) + 8);
        (WorldGenLiquids(lce::blocks::ids::FLOWING_LAVA_ID)).generate(chunk, rng, this->chunkPos.add(x, y, z));
    }
}

void BiomeDecorator::generateOres(ChunkPrimer *chunk, RNG &rng) {
    this->genStandardOre1(chunk, rng, this->dirtGen, STONE_SUBS_COUNT, ORES_MIN_HEIGHT, HIGH_ORES_MAX_HEIGHT);
    this->genStandardOre1(chunk, rng, this->gravelGen, UNCOMMON_ORES_COUNT, ORES_MIN_HEIGHT, HIGH_ORES_MAX_HEIGHT);
    this->genStandardOre1(chunk, rng, this->dioriteGen, STONE_SUBS_COUNT, ORES_MIN_HEIGHT, STONE_SUBS_MAX_HEIGHT);
    this->genStandardOre1(chunk, rng, this->graniteGen, STONE_SUBS_COUNT, ORES_MIN_HEIGHT, STONE_SUBS_MAX_HEIGHT);
    this->genStandardOre1(chunk, rng, this->andesiteGen, STONE_SUBS_COUNT, ORES_MIN_HEIGHT, STONE_SUBS_MAX_HEIGHT);
    this->genStandardOre1(chunk, rng, this->coalGen, COMMON_ORES_COUNT, ORES_MIN_HEIGHT, HIGH_ORES_MAX_HEIGHT);
    this->genStandardOre1(chunk, rng, this->ironGen, COMMON_ORES_COUNT, ORES_MIN_HEIGHT, IRON_ORE_MAX_HEIGHT);
    this->genStandardOre1(chunk, rng, this->goldGen, RARE_ORES_COUNT, ORES_MIN_HEIGHT, GOLD_ORE_MAX_HEIGHT);
    this->genStandardOre1(chunk, rng, this->redstoneGen, UNCOMMON_ORES_COUNT, ORES_MIN_HEIGHT, DIAMOND_ORE_MAX_HEIGHT);
    this->genStandardOre1(chunk, rng, this->diamondGen, VERY_RARE_ORES_COUNT, ORES_MIN_HEIGHT, DIAMOND_ORE_MAX_HEIGHT);
    this->genStandardOre2(chunk, rng, this->lapisGen, VERY_RARE_ORES_COUNT, LAPIS_CENTER_HEIGHT, LAPIS_SPREAD);
}

void BiomeDecorator::genStandardOre1(ChunkPrimer *chunk, RNG &rng, const WorldGenMinable *gen, const int count,
                                     const int minHeight, const int maxHeight) const {
    if (0 < count) {
        for (int bl = count; bl != 0; bl--) {
            Pos3D blockPos = this->chunkPos.add(rng.nextInt(16), rng.nextInt(minHeight, maxHeight), rng.nextInt(16));
            gen->generate(chunk, rng, blockPos);
        }
    }
}

void BiomeDecorator::genStandardOre2(ChunkPrimer *chunk, RNG &rng, WorldGenMinable *gen, const int count,
                                     const int centerHeight, const int spread) const {
    if (0 < count) {
        for (int bl = count; bl != 0; bl--) {
            Pos3D blockPos = this->chunkPos.add(rng.nextInt(16),
                                                (rng.nextInt(spread) + rng.nextInt(spread) + centerHeight) -
                                                spread, rng.nextInt(16));
            gen->generate(chunk, rng, blockPos);
        }
    }
}
