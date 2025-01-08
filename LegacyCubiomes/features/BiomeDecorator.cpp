#include "BiomeDecorator.hpp"

#include "LegacyCubiomes/chunk_generator/biome.hpp"
#include "LegacyCubiomes/chunk_generator/World.hpp"

#include "WorldGenerator/WorldGenCucurbitsBlock.hpp"
#include "WorldGenerator/WorldGenDeadBush.hpp"
#include "WorldGenerator/WorldGenLiquids.hpp"
#include "WorldGenerator/WorldGenMinable.hpp"
#include "WorldGenerator/WorldGenTrees.hpp"

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

void BiomeDecorator::decorate(World *world, Biome *biome, RNG &rng, int posX, int posZ) {
    if (this->decorating) {
        return;
    }

    this->decorating = true;
    this->chunkBlockPos = {posX, 0, posZ};
    this->dirtGen = new WorldGenMinable(&lce::blocks::BlocksInit::DIRT, STONE_SUBS_SIZE);
    this->gravelGen = new WorldGenMinable(&lce::blocks::BlocksInit::GRAVEL, STONE_SUBS_SIZE);
    this->graniteGen = new WorldGenMinable(&lce::blocks::BlocksInit::GRANITE, STONE_SUBS_SIZE);
    this->dioriteGen = new WorldGenMinable(&lce::blocks::BlocksInit::DIORITE, STONE_SUBS_SIZE);
    this->andesiteGen = new WorldGenMinable(&lce::blocks::BlocksInit::ANDESITE, STONE_SUBS_SIZE);
    this->coalGen = new WorldGenMinable(&lce::blocks::BlocksInit::COAL_ORE, COAL_SIZE);
    this->ironGen = new WorldGenMinable(&lce::blocks::BlocksInit::IRON_ORE, IRON_SIZE);
    this->goldGen = new WorldGenMinable(&lce::blocks::BlocksInit::GOLD_ORE, GOLD_SIZE);
    this->redstoneGen = new WorldGenMinable(&lce::blocks::BlocksInit::REDSTONE_ORE, REDSTONE_SIZE);
    this->diamondGen = new WorldGenMinable(&lce::blocks::BlocksInit::DIAMOND_ORE, DIAMOND_SIZE);
    this->lapisGen = new WorldGenMinable(&lce::blocks::BlocksInit::LAPIS_LAZULI_ORE, LAPIS_SIZE);
    this->genDecorations(world, biome, rng);
    this->decorating = false;
}

void BiomeDecorator::decorate(World *world, Biome *biome, RNG &rng, const Pos2D &pos) {
    this->decorate(world, biome, rng, pos.x, pos.z);
}

void BiomeDecorator::genDecorations(World *world, Biome *biome, RNG &rng) {
    this->generateOres(world, rng);

    for (int i = 0; i < this->sandPatchesPerChunk; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        this->sandGen->generate(world, rng, world->getTopSolidOrLiquidBlock(this->chunkBlockPos.add(x, 0, z)));
    }

    for (int i = 0; i < this->clayPerChunk; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        this->clayGen->generate(world, rng, world->getTopSolidOrLiquidBlock(this->chunkBlockPos.add(x, 0, z)));
    }

    for (int i = 0; i < this->gravelPatchesPerChunk; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        this->gravelAsSandGen->generate(world, rng, world->getTopSolidOrLiquidBlock(this->chunkBlockPos.add(x, 0, z)));
    }

    int trees = this->treesPerChunk;
    if (rng.nextFloat() < this->extraTreeChance) {
        ++trees;
    }
    for (int i = 0; i < trees; ++i) {
        /*if (this->chunkBlockPos.getX() >> 4 == -9 && this->chunkBlockPos.getZ() >> 4 == 11) {
            std::cout << "Tree seed: " << rng.nextLongI() << std::endl;
        }*/
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        auto *tree = biome->genBigTreeChance(rng);
        tree->setDecorationDefaults();
        Pos3D blockPos = world->getHeight(this->chunkBlockPos.add(x, 0, z));
        /*if (this->chunkBlockPos.getX() >> 4 == -9 && this->chunkBlockPos.getZ() >> 4 == 11) {
            std::cout << "Tree pos: " << blockPos << std::endl;
        }*/
        if (tree->generate(world, rng, blockPos)) {
            tree->generateSaplings(world, rng, blockPos);
        }
    }

    for (int i = 0; i < this->bigMushroomsPerChunk; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        this->bigMushroomGen->generate(world, rng, world->getHeight(this->chunkBlockPos.add(x, 0, z)));
    }

    for (int i = 0; i < this->flowersPerChunk; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(world->getHeight(this->chunkBlockPos.add(x, 0, z)).getY() + 32);

        Pos3D blockPos = this->chunkBlockPos.add(x, y, z);
        BlockFlower::EnumFlowerType flowerType = biome->pickRandomFlower(rng, blockPos.convert2D());

        c_int flowerBlock = flowerType == BlockFlower::EnumFlowerType::DANDELION
                                ? lce::blocks::ids::DANDELION_ID
                                : lce::blocks::ids::POPPY_ID;
        this->flowerGen->setGeneratedBlock(flowerBlock, flowerType);
        this->flowerGen->generate(world, rng, blockPos);
    }

    for (int i = 0; i < this->grassPerChunk; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(world->getHeight(this->chunkBlockPos.add(x, 0, z)).getY() * 2);
        biome->getRandomWorldGenForGrass(rng)->generate(world, rng, this->chunkBlockPos.add(x, y, z));
    }

    //seagrass here

    for (int i = 0; i < this->deadBushPerChunk; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(world->getHeight(this->chunkBlockPos.add(x, 0, z)).getY() * 2);
        (WorldGenDeadBush()).generate(world, rng, this->chunkBlockPos.add(x, y, z));
    }

    for (int i = 0; i < this->waterlilyPerChunk; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(world->getHeight(this->chunkBlockPos.add(x, 0, z)).getY() * 2);

        Pos3D blockPos = this->chunkBlockPos.add(x, y, z);
        while (blockPos.getY() > 0) {
            blockPos = blockPos.down();
            if (!world->isAirBlock(blockPos)) {
                break;
            }
        }

        this->waterlilyGen->generate(world, rng, blockPos.up());
    }

    for (int i = 0; i < this->mushroomsPerChunk; ++i) {
        if (rng.nextInt(4) == 0) {
            c_int x = rng.nextInt(16) + 8;
            c_int z = rng.nextInt(16) + 8;
            this->mushroomBrownGen->generate(world, rng, world->getHeight(this->chunkBlockPos.add(x, 0, z)));
        }

        if (rng.nextInt(8) == 0) {
            c_int x = rng.nextInt(16) + 8;
            c_int z = rng.nextInt(16) + 8;
            c_int y = rng.nextInt(world->getHeight(this->chunkBlockPos.add(x, 0, z)).getY() * 2);
            this->mushroomRedGen->generate(world, rng, this->chunkBlockPos.add(x, y, z));
        }
    }

    if (rng.nextInt(4) == 0) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(world->getHeight(this->chunkBlockPos.add(x, 0, z)).getY() * 2);
        this->mushroomBrownGen->generate(world, rng, this->chunkBlockPos.add(x, y, z));
    }

    if (rng.nextInt(8) == 0) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(world->getHeight(this->chunkBlockPos.add(x, 0, z)).getY() * 2);
        this->mushroomRedGen->generate(world, rng, this->chunkBlockPos.add(x, y, z));
    }

    //kelp here

    for (int i = 0; i < this->reedsPerChunk + 10; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(world->getHeight(this->chunkBlockPos.add(x, 0, z)).getY() * 2);
        this->reedGen->generate(world, rng, this->chunkBlockPos.add(x, y, z));
    }

    if (rng.nextInt(32) == 0) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(world->getHeight(this->chunkBlockPos.add(x, 0, z)).getY() * 2);
        (WorldGenCucurbitsBlock(lce::blocks::ids::CARVED_PUMPKIN_ID, true)).generate(
            world, rng, this->chunkBlockPos.add(x, y, z));
    }

    for (int i = 0; i < this->cactiPerChunk; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(world->getHeight(this->chunkBlockPos.add(x, 0, z)).getY() * 2);
        this->cactusGen->generate(world, rng, this->chunkBlockPos.add(x, y, z));
    }

    for (int i = 0; i < 50; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(rng.nextInt(120) + 8);
        WorldGenLiquids(lce::blocks::ids::FLOWING_WATER_ID).generate(world, rng, this->chunkBlockPos.add(x, y, z));
    }

    for (int i = 0; i < 20; ++i) {
        c_int x = rng.nextInt(16) + 8;
        c_int z = rng.nextInt(16) + 8;
        c_int y = rng.nextInt(rng.nextInt(rng.nextInt(112) + 8) + 8);
        WorldGenLiquids(lce::blocks::ids::FLOWING_LAVA_ID).generate(world, rng, this->chunkBlockPos.add(x, y, z));
    }
}

void BiomeDecorator::generateOres(World *world, RNG &rng) {
    this->genStandardOre1(world, rng, this->dirtGen, STONE_SUBS_COUNT, ORES_MIN_HEIGHT, HIGH_ORES_MAX_HEIGHT);
    this->genStandardOre1(world, rng, this->gravelGen, UNCOMMON_ORES_COUNT, ORES_MIN_HEIGHT, HIGH_ORES_MAX_HEIGHT);
    this->genStandardOre1(world, rng, this->dioriteGen, STONE_SUBS_COUNT, ORES_MIN_HEIGHT, STONE_SUBS_MAX_HEIGHT);
    this->genStandardOre1(world, rng, this->graniteGen, STONE_SUBS_COUNT, ORES_MIN_HEIGHT, STONE_SUBS_MAX_HEIGHT);
    this->genStandardOre1(world, rng, this->andesiteGen, STONE_SUBS_COUNT, ORES_MIN_HEIGHT, STONE_SUBS_MAX_HEIGHT);
    this->genStandardOre1(world, rng, this->coalGen, COMMON_ORES_COUNT, ORES_MIN_HEIGHT, HIGH_ORES_MAX_HEIGHT);
    this->genStandardOre1(world, rng, this->ironGen, COMMON_ORES_COUNT, ORES_MIN_HEIGHT, IRON_ORE_MAX_HEIGHT);
    this->genStandardOre1(world, rng, this->goldGen, RARE_ORES_COUNT, ORES_MIN_HEIGHT, GOLD_ORE_MAX_HEIGHT);
    this->genStandardOre1(world, rng, this->redstoneGen, UNCOMMON_ORES_COUNT, ORES_MIN_HEIGHT, DIAMOND_ORE_MAX_HEIGHT);
    this->genStandardOre1(world, rng, this->diamondGen, VERY_RARE_ORES_COUNT, ORES_MIN_HEIGHT, DIAMOND_ORE_MAX_HEIGHT);
    this->genStandardOre2(world, rng, this->lapisGen, VERY_RARE_ORES_COUNT, LAPIS_CENTER_HEIGHT, LAPIS_SPREAD);
}

void BiomeDecorator::genStandardOre1(World *world, RNG &rng, const WorldGenMinable *gen, const int count,
                                     const int minHeight, const int maxHeight) const {
    if (0 < count) {
        for (int bl = count; bl != 0; bl--) {
            int x_off;
            int y_off;
            int z_off;
            if (world->getGenerator()->getConsole() != lce::CONSOLE::XBOX360 && world->getGenerator()->getConsole() != lce::CONSOLE::XBOX1) {
                x_off = rng.nextInt(16);
                y_off = rng.nextInt(maxHeight - minHeight) + minHeight;
                z_off = rng.nextInt(16);
            }
            else {
                z_off = rng.nextInt(16);
                y_off = rng.nextInt(maxHeight - minHeight) + minHeight;
                x_off = rng.nextInt(16);
            }

            gen->generate(world, rng, this->chunkBlockPos.add(x_off, y_off, z_off));
        }
    }
}

void BiomeDecorator::genStandardOre2(World *world, RNG &rng, const WorldGenMinable *gen, const int count,
                                     const int centerHeight, const int spread) const {
    if (0 < count) {
        for (int bl = count; bl != 0; bl--) {
            int x_off;
            int y_off;
            int z_off;
            if (world->getGenerator()->getConsole() != lce::CONSOLE::XBOX360 && world->getGenerator()->getConsole() != lce::CONSOLE::XBOX1) {
                x_off = rng.nextInt(16);
                y_off = rng.nextInt(spread) + rng.nextInt(spread) + centerHeight;
                z_off = rng.nextInt(16);
            }
            else {
                z_off = rng.nextInt(16);
                y_off = rng.nextInt(spread) + rng.nextInt(spread) + centerHeight;
                x_off = rng.nextInt(16);
            }
            Pos3D blockPos = this->chunkBlockPos.add(x_off, y_off, z_off);
            gen->generate(world, rng, blockPos);
        }
    }
}
