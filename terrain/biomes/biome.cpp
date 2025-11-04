#include "biome.hpp"
#include "terrain/ChunkPrimer.hpp"
#include "terrain/World.hpp"

#include "BiomeDecorator.hpp"
#include "biome_t.hpp"
#include "terrain/color/Colors.hpp"
#include "terrain/decorators/WorldGenBigMushroom.hpp"
#include "terrain/decorators/WorldGenBigTree.hpp"
#include "terrain/decorators/WorldGenBirchTree.hpp"
#include "terrain/decorators/WorldGenBlockBlob.hpp"
#include "terrain/decorators/WorldGenCanopyTree.hpp"
#include "terrain/decorators/WorldGenCucurbitsBlock.hpp"
#include "terrain/decorators/WorldGenDoublePlant.hpp"
#include "terrain/decorators/WorldGenFlowers.hpp"
#include "terrain/decorators/WorldGenMegaJungle.hpp"
#include "terrain/decorators/WorldGenMegaPineTree.hpp"
#include "terrain/decorators/WorldGenSavannaTree.hpp"
#include "terrain/decorators/WorldGenShrub.hpp"
#include "terrain/decorators/WorldGenSwamp.hpp"
#include "terrain/decorators/WorldGenTaiga1.hpp"
#include "terrain/decorators/WorldGenTaiga2.hpp"
#include "terrain/decorators/WorldGenTallGrass.hpp"
#include "terrain/decorators/WorldGenTrees.hpp"
#include "terrain/decorators/WorldGenVines.hpp"

#pragma region Definitions

const std::map<biome_t, int> Biome::SNOW_BIOMES = {
        {biome_t::extreme_hills, 90},
        {biome_t::taiga, 120},
        {biome_t::taiga_hills, 120},
        {biome_t::extreme_hills_edge, 90},
        {biome_t::stone_beach, 90},
        {biome_t::cold_beach, 0},
        {biome_t::mega_taiga, 150},
        {biome_t::mega_taiga_hills, 150},
        {biome_t::extreme_hills, 90},
        {biome_t::extreme_hills_plus_trees, 90},
        {biome_t::extreme_hills_mutated, 90},
        {biome_t::taiga_mutated, 120},
        {biome_t::mega_spruce_taiga, 120},
        {biome_t::mega_taiga_hills, 120},
        // Biomes that always snow
        {biome_t::legacy_frozen_ocean, -10},
        {biome_t::frozen_river, -10},
        {biome_t::ice_plains, -10},
        {biome_t::ice_mountains, -10},
        {biome_t::cold_taiga, -10},
        {biome_t::cold_taiga_hills, -10},
        {biome_t::frozen_ocean, -10},
        {biome_t::deep_frozen_ocean, -10},
        {biome_t::ice_plains_spikes, -10},
        {biome_t::cold_taiga_mutated, -10},
};

const NoiseGeneratorPerlin Biome::TEMPERATURE_NOISE = NoiseGeneratorPerlin(RNG(1234ULL), 1);
const NoiseGeneratorPerlin Biome::INFO_NOISE = NoiseGeneratorPerlin(RNG(2345ULL), 1);

const WorldGenDoublePlant Biome::DOUBLE_PLANT_GENERATOR;
const WorldGenTrees Biome::TREE_FEATURE;
const WorldGenBigTree Biome::BIG_TREE_FEATURE;

const WorldGenBirchTree BiomeForest::SUPER_BIRCH_TREE = WorldGenBirchTree(true);
const WorldGenBirchTree BiomeForest::BIRCH_TREE = WorldGenBirchTree(false);
const WorldGenCanopyTree BiomeForest::ROOF_TREE;

const WorldGenTaiga1 BiomeTaiga::PINE_GENERATOR;
const WorldGenTaiga2 BiomeTaiga::SPRUCE_GENERATOR;
const WorldGenTallGrass BiomeTaiga::FERN_GENERATOR = WorldGenTallGrass(BlockTallGrass::EnumType::FERN);
const WorldGenTallGrass BiomeTaiga::GRASS_GENERATOR = WorldGenTallGrass(BlockTallGrass::EnumType::GRASS);
const WorldGenMegaPineTree BiomeTaiga::MEGA_PINE_GENERATOR = WorldGenMegaPineTree(false);
const WorldGenMegaPineTree BiomeTaiga::MEGA_SPRUCE_GENERATOR = WorldGenMegaPineTree(true);

const WorldGenBlockBlob BiomeTaiga::FOREST_ROCK_GENERATOR =
        WorldGenBlockBlob(lce::BlocksInit::MOSS_STONE.getState(), 0);

const WorldGenSwamp BiomeSwamp::SWAMP_FEATURE;

const WorldGenMegaJungle BiomeJungle::MEGA_JUNGLE_FEATURE = WorldGenMegaJungle(10, 20, lce::BlocksInit::JUNGLE_WOOD.getState(), lce::BlocksInit::JUNGLE_LEAVES.getState());
const WorldGenTrees BiomeJungle::JUNGLE_TREES[BiomeJungle::JUNGLE_TREE_HEIGHT_VARIATION] = {
    WorldGenTrees(4, lce::BlocksInit::JUNGLE_WOOD.getState(), lce::BlocksInit::JUNGLE_LEAVES.getState(), true),
    WorldGenTrees(5, lce::BlocksInit::JUNGLE_WOOD.getState(), lce::BlocksInit::JUNGLE_LEAVES.getState(), true),
    WorldGenTrees(6, lce::BlocksInit::JUNGLE_WOOD.getState(), lce::BlocksInit::JUNGLE_LEAVES.getState(), false),
    WorldGenTrees(7, lce::BlocksInit::JUNGLE_WOOD.getState(), lce::BlocksInit::JUNGLE_LEAVES.getState(), false),
    WorldGenTrees(8, lce::BlocksInit::JUNGLE_WOOD.getState(), lce::BlocksInit::JUNGLE_LEAVES.getState(), false),
    WorldGenTrees(9, lce::BlocksInit::JUNGLE_WOOD.getState(), lce::BlocksInit::JUNGLE_LEAVES.getState(), false),
    WorldGenTrees(10, lce::BlocksInit::JUNGLE_WOOD.getState(), lce::BlocksInit::JUNGLE_LEAVES.getState(), false)
};

const WorldGenSavannaTree BiomeSavanna::SAVANNA_TREE;

std::map<biome_t, Biome *> Biome::registry;
#pragma endregion

Biome::~Biome() { delete decorator; }

#pragma region BiomeDefaults

BiomeDecorator *Biome::createBiomeDecorator() const {
    return new BiomeDecorator();
}

void Biome::registerBiomes() {
    registerBiome(255, new BiomeNone("None", -1.0F, 0.1F, false, 0.5F, 0xA5333333));
    registerBiome(0, new BiomeOcean("Ocean", -1.0F, 0.1F, false, 0.5F, 0xA5D48717));
    registerBiome(1, new BiomePlains(false, "Plains", 0.125F, 0.05F, false, 0.8F));
    registerBiome(2, new BiomeDesert("Desert", 0.125F, 0.05F, false, 2.0F, 0xA598A532));
    registerBiome(3, new BiomeHills(BiomeHills::Type::NORMAL, "Extreme Hills", 1.0, 0.5F, false, 0.2F, 0xA5F77B00));
    registerBiome(4, new BiomeForest(BiomeForest::Type::NORMAL, "Forest", 0.1F, 0.2F, false, 0.7F, 0xA5F2971E));
    registerBiome(5, new BiomeTaiga(BiomeTaiga::Type::NORMAL, "Taiga", 0.2F, 0.2F, false, 0.25F, 0xA5827028));
    registerBiome(6, new BiomeSwamp("Swampland", -0.2, 0.1F, false, 0.8F, 0xFF59654C));
    registerBiome(7, new BiomeRiver("River", -0.5F, 0.0F, false, 0.5F, 0xA5FF8400));
    registerBiome(8, new Biome("Nether", 0.1F, 0.2F, false, 2.0F, 0.0F, 0xA5575990));
    registerBiome(9, new Biome("The End", 0.1F, 0.2F, false, 0.5F, 0.0F, 0xA59E5262));
    registerBiome(10, new BiomeOcean("Frozen Ocean", -1.0F, 0.1F, true, 0.0F, 0xA5B57025));
    registerBiome(11, new BiomeRiver("Frozen River", -0.5F, 0.0F, true, 0.0F, 0xA5905318));
    registerBiome(12, new BiomeSnow(false, "Ice Plains", 0.125F, 0.05F, true, 0.0F, 0xA59B5514));
    registerBiome(13, new BiomeSnow(false, "Ice Mountains", 0.45F, 0.3F, true, 0.0F, 0xA5A75611));
    registerBiome(14, new BiomeMushroomIsland("Mushroom Island", 0.2F, 0.3F, false, 0.9F, 0xA597898A));
    registerBiome(15, new BiomeMushroomIsland("Mushroom Island Shore", 0.0F, 0.025F, false, 0.9F, 0xA5938181));
    registerBiome(16, new BiomeBeach("Beach", 0.0F, 0.025F, false, 0.8F, 0.4F, 0xA5AB7C15));
    registerBiome(17, new BiomeDesert("Desert Hills", 0.45, 0.3F, false, 2.0F, 0xA5A17A1A));
    registerBiome(18, new BiomeForest(BiomeForest::Type::NORMAL, "Forest Hills", 0.45F, 0.3F, false, 0.7F, 0xA5D16B05));
    registerBiome(19, new BiomeTaiga(BiomeTaiga::Type::NORMAL, "Taiga Hills", 0.45F, 0.3F, false, 0.25F, 0xA5836523));
    registerBiome(20, new BiomeHills(BiomeHills::Type::EXTRA_TREES, "Extreme Hills Edge", 0.8F, 0.3F, false, 0.2F,
                                     0xA5D55C04));
    registerBiome(21, new BiomeJungle(false, "Jungle", 0.1F, 0.2F, false, 0.95F, 0.9F, 0xA5C5A214));
    registerBiome(22, new BiomeJungle(false, "Jungle Hills", 0.45F, 0.3F, false, 0.95F, 0.9F, 0xA5D89E1B));
    registerBiome(23, new BiomeJungle(true, "Jungle Edge", 0.1F, 0.2F, false, 0.95F, 0.8F, 0xA5E38A0D));
    registerBiome(24, new BiomeOcean("Deep Ocean", -1.8F, 0.1F, false, 0.5F, 0xA5D48717));
    registerBiome(25, new BiomeStoneBeach("Stone Beach", 0.1F, 0.8F, false, 0.2F));
    registerBiome(26, new BiomeBeach("Cold Beach", 0.0F, 0.025F, true, 0.05F, 0.3F, 0xA5A56314));
    registerBiome(27, new BiomeForest(BiomeForest::Type::BIRCH, "Birch Forest", 0.1F, 0.2F, false, 0.6F, 0xA5CE7706));
    registerBiome(28, new BiomeForest(BiomeForest::Type::BIRCH, "Birch Forest Hills", 0.45F, 0.3F, false, 0.6F,
                                      0xA5C4740A));
    registerBiome(29, new BiomeForest(BiomeForest::Type::ROOFED, "Roofed Forest", 0.1F, 0.2F, false, 0.7F, 0xA5D16C3B));
    registerBiome(30, new BiomeTaiga(BiomeTaiga::Type::NORMAL, "Cold Taiga", 0.2F, 0.2F, true, -0.5F, 0xA5835E20));
    registerBiome(
        31, new BiomeTaiga(BiomeTaiga::Type::NORMAL, "Cold Taiga Hills", 0.45F, 0.3F, true, -0.5F, 0xA5785B24));
    registerBiome(32, new BiomeTaiga(BiomeTaiga::Type::MEGA, "Mega Taiga", 0.2F, 0.2F, false, 0.3F, 0xA5776D2D));
    registerBiome(33, new BiomeTaiga(BiomeTaiga::Type::MEGA, "Mega Taiga Hills", 0.45F, 0.3F, false, 0.3F));
    registerBiome(34, new BiomeHills(BiomeHills::Type::EXTRA_TREES, "Extreme Hills Plus Trees", 1.0F, 0.5F, false, 0.2F,
                                     0xA5AB630E));
    registerBiome(35, new BiomeSavanna("Savanna", 0.125F, 0.05F, false, 1.2F, 0xA59C8B2C));
    registerBiome(36, new BiomeSavanna("Savanna Plateau", 1.5F, 0.025F, false, 1.0F, 0xA5A89025));
    registerBiome(37, new BiomeMesa(false, false, "Mesa", 0.1F, 0.2F, false, 2.0F, 0xA5817F4E));
    registerBiome(38, new BiomeMesa(false, true, "Mesa Plateau Stone", 1.5F, 0.025F, false, 2.0F, 0xA59E8055));
    registerBiome(39, new BiomeMesa(false, false, "Mesa Plateau", 1.5F, 0.025F, false, 2.0F, 0xA59E8055));

    registerBiome(40, new BiomeOcean("Warm Ocean", -1.0F, 0.1F, false, 0.5F, 0x8CE5B002));
    registerBiome(41, new BiomeOcean("Deep Warm Ocean", -1.8F, 0.1F, false, 0.5F, 0xA5E5B002));
    registerBiome(42, new BiomeOcean("Lukewarm Ocean", -1.0F, 0.1F, false, 0.5F, 0xA5DB960D));
    registerBiome(43, new BiomeOcean("Deep Lukewarm Ocean", -1.8F, 0.1F, false, 0.5F, 0xA5DB960D));
    registerBiome(44, new BiomeOcean("Cold Ocean", -1.0F, 0.1F, false, 0.5F, 0xA5C98020));
    registerBiome(45, new BiomeOcean("Deep Cold Ocean", -1.8F, 0.1F, false, 0.5F, 0xA5C98020));
    registerBiome(46, new BiomeOcean("Frozen Ocean", -1.0F, 0.1F, false, 0.0F, 0xA5B57025));
    registerBiome(47, new BiomeOcean("Deep Frozen Ocean", -1.8F, 0.1F, false, 0.0F, 0xA5B57025));
    // mutated
    registerBiome(129, new BiomePlains(true, "Sunflower Plains", 0.125F, 0.05F, false, 0.8F));
    registerBiome(130, new BiomeDesert("Mutated Desert", 0.225F, 0.25F, false, 2.0F, 0x80CA8900));
    registerBiome(131, new BiomeHills(BiomeHills::Type::MUTATED, "Mutated Extreme Hills", 1.0F, 0.5F, false, 0.2F,
                                      0xA5AB630E));
    registerBiome(132, new BiomeForest(BiomeForest::Type::FLOWER, "Flower Forest", 0.1F, 0.4F, false, 0.7F,
                                       0xA5CCA320));
    registerBiome(133, new BiomeTaiga(BiomeTaiga::Type::NORMAL, "Mutated Taiga", 0.3F, 0.4F, false, 0.25F, 0xA5826B1E));
    registerBiome(134, new BiomeSwamp("Mutated Swampland", -0.1F, 0.3F, false, 0.8F, 0xFF56614C));
    registerBiome(140, new BiomeSnow(true, "Ice Plains Spikes", 0.425F, 0.45000002F, true, 0.0F, 0xA59B5514));
    registerBiome(149, new BiomeJungle(false, "Mutated Jungle", 0.2F, 0.4F, false, 0.95F, 0.9F, 0xA5D89E1B));
    registerBiome(151, new BiomeJungle(true, "Mutated Jungle Edge", 0.2F, 0.4F, false, 0.95F, 0.8F, 0x80CA8900));
    registerBiome(155, new BiomeForestMutated("Mutated Birch Forest", 0.2F, 0.4F, false, 0.6F));
    registerBiome(156, new BiomeForestMutated("Mutated Birch Forest Hills", 0.55F, 0.5F, false, 0.6F));
    registerBiome(157, new BiomeForest(BiomeForest::Type::ROOFED, "Mutated Roofed Forest", 0.2F, 0.4F, false, 0.7F,
                                       0x80CA8900));
    registerBiome(158, new BiomeTaiga(BiomeTaiga::Type::NORMAL, "Mutated Cold Taiga", 0.3F, 0.4F, true, -0.5F,
                                      0xA5835E20));
    registerBiome(160, new BiomeTaiga(BiomeTaiga::Type::MEGA_SPRUCE, "Mega Spruce Taiga", 0.2F, 0.2F, false, 0.25F,
                                      0xA5776D2D));
    registerBiome(161, new BiomeTaiga(BiomeTaiga::Type::MEGA_SPRUCE, "Mega Taiga Hills", 0.2F, 0.2F, false, 0.25F,
                                      0xA5786328));
    registerBiome(162, new BiomeHills(BiomeHills::Type::MUTATED, "Mutated Extreme Hills Plus Trees", 1.0F, 0.5F, false,
                                      0.2F, 0xA5AB630E));
    registerBiome(163, new BiomeSavannaMutated("Mutated Savanna", 0.3625F, 1.225F, false, 1.1F, 0xA5A89025));
    registerBiome(164, new BiomeSavannaMutated("Mutated Savanna Plateau", 1.05F, 1.2125001F, false, 1.0F, 0x80CA8900));
    registerBiome(165, new BiomeMesa(true, false, "Mesa Bryce", 0.1F, 0.2F, false, 2.0F, 0xA5997F49));
    registerBiome(166, new BiomeMesa(false, true, "Mutated Mesa Plateau Stone", 0.45F, 0.3F, false, 2.0F));
    registerBiome(167, new BiomeMesa(false, false, "Mutated Mesa Plateau", 0.45F, 0.3F, false, 2.0F));
}

const WorldGenAbstractTree *Biome::genBigTreeChance(RNG &rng) const {
    return (rng.nextInt(10) == 0 ? (WorldGenAbstractTree *) &BIG_TREE_FEATURE : (WorldGenAbstractTree *) &TREE_FEATURE);
}

const AbstractWorldGenerator *Biome::getRandomWorldGenForGrass(MU RNG &rng) const {
    return new WorldGenTallGrass(BlockTallGrass::EnumType::GRASS);
}

BlockFlower::EnumFlowerType Biome::pickRandomFlower(RNG &rng, MU const Pos2D &pos) const {
    return rng.nextInt(3) > 0 ? BlockFlower::EnumFlowerType::DANDELION : BlockFlower::EnumFlowerType::POPPY;
}

bool Biome::isSnowyBiome() const {
    return SNOW_BIOMES.contains(biomeID);
}

bool Biome::hasIdealTemperature(const Pos3D& pos) const {
    if (!this->isSnowyBiome())
        return false;

    int chanceStart = SNOW_BIOMES.at(biomeID);
    if (pos.getY() < chanceStart)
        return false;

    int alwaysStart = chanceStart + SNOW_CHANCE_HEIGHT;
    if (pos.getY() >= alwaysStart)
        return true;

    return getFloatTemperature(pos) >= 0.15F;
}

float Biome::getFloatTemperature(const Pos3D &pos) const {
    if (pos.getY() > 64) {
        c_auto f = (float) (TEMPERATURE_NOISE.getValue((float) pos.getX() / 8.0F, (float) pos.getZ() / 8.0F) * 4.0);
        return this->temperature - (f + (float) pos.getY() - 64.0F) * 0.05F / 30.0F;
    }
    return this->temperature;
}

void Biome::generateBiomeTerrain(RNG &rng, ChunkPrimer *chunkPrimerIn, int x, int z, double noiseVal) const {
    constexpr int seaLevel = 63; // sea level
    lce::BlockState top = this->topBlock;
    lce::BlockState filler = this->fillerBlock;
    int j = -1;
    c_int k = (int) (noiseVal / 3.0 + 3.0 + rng.nextDouble() * 0.25);
    c_int l = x & 15;
    c_int i1 = z & 15;
    Pos3D blockPos;

    for (int j1 = 255; j1 >= 0; --j1) {
        if (j1 == 0) break;

        if (rng.nextInt(2) + 1 >= j1 - 1) chunkPrimerIn->setBlockId(i1, j1 - 1, l, lce::blocks::BEDROCK_ID);
        else {
            c_u16 currentBlockId = chunkPrimerIn->getBlockId(i1, j1, l);

            if (currentBlockId == lce::blocks::AIR_ID) // if air
                j = -1;
            else if (currentBlockId == lce::blocks::STONE_ID) {
                if (j == -1) {
                    if (k <= 0) {
                        top = lce::BlocksInit::AIR.getState();
                        filler = lce::BlocksInit::STONE.getState();
                    } else if (j1 >= seaLevel - 4 && j1 <= seaLevel + 1) {
                        top = this->topBlock;
                        filler = this->fillerBlock;
                    }

                    if (j1 < seaLevel && top == lce::BlocksInit::AIR.getState()) {
                        blockPos.setPos(x, j1, z);
                        if (this->getFloatTemperature(blockPos) < 0.15F) top = lce::BlocksInit::ICE.getState();
                        else
                            top = lce::BlocksInit::STILL_WATER.getState();
                    }

                    j = k;

                    if (j1 >= seaLevel - 1) chunkPrimerIn->setBlock(i1, j1, l, top);
                    else if (j1 < seaLevel - 7 - k) {
                        top = lce::BlocksInit::AIR.getState();
                        filler = lce::BlocksInit::STONE.getState();
                        chunkPrimerIn->setBlockId(i1, j1, l, lce::blocks::GRAVEL_ID);
                    } else
                        chunkPrimerIn->setBlock(i1, j1, l, filler);
                } else if (j > 0) {
                    --j;
                    chunkPrimerIn->setBlock(i1, j1, l, filler);

                    if (j == 0 && filler.getID() == lce::blocks::SAND_ID && k > 1) {
                        j = rng.nextInt(4) + std::max(0, j1 - 63);
                        filler = filler.getDataTag() == 1
                                     ? lce::BlocksInit::RED_SANDSTONE.getState()
                                     : lce::BlocksInit::SANDSTONE.getState();
                    }
                }
            }
        }
    }
}

#pragma endregion

#pragma region TerrainBlocks

void Biome::genTerrainBlocks(MU i64 worldSeed, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x, c_int z,
                             c_double noiseVal) {
    this->generateBiomeTerrain(rng, chunkPrimerIn, x, z, noiseVal);
}

void BiomeHills::genTerrainBlocks(MU i64 worldSeed, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x, c_int z,
                                  c_double noiseVal) {
    this->topBlock = lce::BlocksInit::GRASS.getState();
    this->fillerBlock = lce::BlocksInit::DIRT.getState();
    if ((noiseVal < -1.0 || noiseVal > 2.0) && this->type == BiomeHills::Type::MUTATED) {
        this->topBlock = lce::BlocksInit::GRAVEL.getState();
        this->fillerBlock = lce::BlocksInit::GRAVEL.getState();
    } else if (noiseVal > 1.0 && this->type != BiomeHills::Type::EXTRA_TREES) {
        this->topBlock = lce::BlocksInit::STONE.getState();
        this->fillerBlock = lce::BlocksInit::STONE.getState();
    }

    this->generateBiomeTerrain(rng, chunkPrimerIn, x, z, noiseVal);
}

void BiomeTaiga::genTerrainBlocks(MU i64 worldSeed, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x, c_int z,
                                  c_double noiseVal) {
    if (this->type == BiomeTaiga::Type::MEGA || this->type == BiomeTaiga::Type::MEGA_SPRUCE) {
        if (noiseVal > 1.75) {
            this->topBlock = lce::BlocksInit::COARSE_DIRT.getState();
        } else if (noiseVal > -0.95) {
            this->topBlock = lce::BlocksInit::PODZOL.getState();
        }
    }

    this->generateBiomeTerrain(rng, chunkPrimerIn, x, z, noiseVal);
}

void BiomeSwamp::genTerrainBlocks(MU i64 worldSeed, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x, c_int z,
                                  c_double noiseVal) {
    c_double d0 = INFO_NOISE.getValue(static_cast<double>(x) * 0.25, static_cast<double>(z) * 0.25);

    if (d0 > 0.0) {
        c_int zOff = x & 15;
        c_int xOff = z & 15;

        for (int y = 255; y >= 0; --y) {
            if (chunkPrimerIn->getBlockId(xOff, y, zOff) != lce::blocks::AIR_ID) {
                if (y == 62 && chunkPrimerIn->getBlockId(xOff, y, zOff) != lce::blocks::STILL_WATER_ID) {
                    chunkPrimerIn->setBlockId(xOff, y, zOff, lce::blocks::STILL_WATER_ID);

                    if (d0 < 0.12) {
                        chunkPrimerIn->setBlockId(xOff, y + 1, zOff, lce::blocks::LILY_PAD_ID);
                    }
                }

                break;
            }
        }
    }

    this->generateBiomeTerrain(rng, chunkPrimerIn, x, z, noiseVal);
}

void BiomeMesa::genTerrainBlocks(c_i64 worldSeedIn, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x,
                                 c_int z, c_double noiseVal) {
    this->worldSeed = worldSeedIn;
    if (this->clayBands.empty()) { this->generateClayBands(worldSeedIn); }

    if (this->pillarNoise.noiseLevels.empty() || this->pillarRoofNoise.noiseLevels.empty()) {
        RNG rng1;
        rng1.setSeed(worldSeedIn);
        this->pillarNoise.setNoiseGeneratorPerlin(rng, 4);
        this->pillarRoofNoise.setNoiseGeneratorPerlin(rng, 1);
    }
    double d4 = 0.0;

    if (this->brycePillars) {
        c_int i = (x & -16) + (z & 15);
        c_int j = (z & -16) + (x & 15);
        c_double d0 = std::min(std::abs(noiseVal), this->pillarNoise.getValue(
                                   static_cast<double>(i) * 0.25, static_cast<double>(j) * 0.25));

        if (d0 > 0.0) {
            double d2 = std::abs(this->pillarRoofNoise.getValue(
                static_cast<double>(i) * 0.001953125, static_cast<double>(j) * 0.001953125));
            d4 = d0 * d0 * 2.5;
            c_double d3 = ceil(d2 * 50.0) + 14.0;

            if (d4 > d3) d4 = d3;

            d4 = d4 + 64.0;
        }
    }

    c_int k1 = x & 15;
    c_int l1 = z & 15;
    constexpr int seaLevel = 63; // sea level
    lce::BlockState filler = this->fillerBlock;
    c_int k = static_cast<int>(noiseVal / 3.0 + 3.0 + rng.nextDouble() * 0.25);
    c_bool flag = cos(noiseVal / 3.0 * PI) > 0.0;
    int l = -1;
    bool flag1 = false;
    int i1 = 0;

    for (int j1 = 255; j1 >= 0; --j1) {
        if (chunkPrimerIn->getBlockId(l1, j1, k1) == lce::blocks::AIR_ID && j1 < static_cast<int>(d4)) {
            chunkPrimerIn->setBlockId(l1, j1, k1, lce::blocks::STONE_ID);
        }
        if (j1 == 0) break;

        if (rng.nextInt(2) + 1 >= j1 - 1) chunkPrimerIn->setBlockId(l1, j1 - 1, k1, lce::blocks::BEDROCK_ID);
        else if (i1 < 15 || this->brycePillars) {
            c_u16 iBlockState1 = chunkPrimerIn->getBlockId(l1, j1, k1);

            if (!iBlockState1) l = -1;
            else if (iBlockState1 == lce::blocks::STONE_ID) {
                if (l == -1) {
                    flag1 = false;

                    if (k <= 0) {
                        filler = lce::BlocksInit::STONE.getState();
                    } else if (j1 >= seaLevel - 4 && j1 <= seaLevel + 1) {
                        filler = this->fillerBlock;
                    }

                    l = k + std::max(0, j1 - seaLevel);

                    if (j1 >= seaLevel - 1) {
                        if (this->hasForest && j1 > 86 + k * 2) {
                            if (flag) chunkPrimerIn->setBlock(l1, j1, k1, lce::BlocksInit::COARSE_DIRT.getState());
                            else
                                chunkPrimerIn->setBlockId(l1, j1, k1, lce::blocks::GRASS_ID);
                        } else if (j1 > seaLevel + 3 + k) {
                            lce::BlockState iBlockState2 = lce::BlocksInit::AIR.getState();

                            if (j1 >= 64 && j1 <= 127) {
                                if (flag) iBlockState2 = lce::BlocksInit::HARDENED_CLAY.getState();
                                else
                                    iBlockState2 = this->getClayBand(x, j1);
                            } else
                                iBlockState2 = lce::BlocksInit::ORANGE_HARDENED_CLAY.getState();

                            chunkPrimerIn->setBlock(l1, j1, k1, iBlockState2);
                        } else {
                            chunkPrimerIn->setBlock(l1, j1, k1, this->topBlock);
                            flag1 = true;
                        }
                    } else {
                        chunkPrimerIn->setBlock(l1, j1, k1, filler);

                        if (filler.getID() == lce::blocks::STAINED_HARDENED_CLAY_ID)
                            chunkPrimerIn->setBlock(l1, j1, k1, lce::BlocksInit::ORANGE_HARDENED_CLAY.getState());
                    }
                } else if (l > 0) {
                    --l;

                    if (flag1) chunkPrimerIn->setBlock(l1, j1, k1, lce::BlocksInit::ORANGE_HARDENED_CLAY.getState());
                    else
                        chunkPrimerIn->setBlock(l1, j1, k1, this->getClayBand(x, j1));
                }

                ++i1;
            }
        }
    }
}

void BiomeMesa::generateClayBands(c_i64 seed) {
    this->clayBands.resize(64, lce::BlocksInit::HARDENED_CLAY.getState());
    RNG rng;
    rng.setSeed(seed);
    this->clayBandsOffsetNoise.setNoiseGeneratorPerlin(rng, 1);

    for (int index = 0; index < 64; ++index) {
        index += rng.nextInt(5) + 1;

        if (index < 64) this->clayBands[index] = lce::BlocksInit::WHITE_HARDENED_CLAY.getState();
    }

    c_int orangeBands = rng.nextInt(4) + 2;

    for (int i = 0; i < orangeBands; ++i) {
        c_int bandLength = rng.nextInt(3) + 1;
        c_int startPos = rng.nextInt(64);

        for (int offset = 0; startPos + offset < 64 && offset < bandLength; ++offset)
            this->clayBands[startPos + offset] = lce::BlocksInit::YELLOW_HARDENED_CLAY.getState();
    }

    c_int brownBands = rng.nextInt(4) + 2;

    for (int i = 0; i < brownBands; ++i) {
        c_int bandLength = rng.nextInt(3) + 2;
        c_int startPos = rng.nextInt(64);

        for (int offset = 0; startPos + offset < 64 && offset < bandLength; ++offset)
            this->clayBands[startPos + offset] = lce::BlocksInit::BROWN_HARDENED_CLAY.getState();
    }

    c_int redBands = rng.nextInt(4) + 2;

    for (int i = 0; i < redBands; ++i) {
        c_int bandLength = rng.nextInt(3) + 1;
        c_int startPos = rng.nextInt(64);

        for (int offset = 0; startPos + offset < 64 && offset < bandLength; ++offset)
            this->clayBands[startPos + offset] = lce::BlocksInit::RED_HARDENED_CLAY.getState();
    }

    c_int grayBands = rng.nextInt(3) + 3;
    int previousPos = 0;

    for (int i = 0; i < grayBands; ++i) {
        previousPos += rng.nextInt(16) + 4;

        for (int offset = 0; previousPos + offset < 64 && offset < 1; ++offset) {
            this->clayBands[previousPos + offset] = lce::BlocksInit::WHITE_HARDENED_CLAY.getState();

            if (previousPos + offset > 1 && rng.nextBoolean())
                this->clayBands[previousPos + offset - 1] = lce::BlocksInit::LIGHT_GRAY_HARDENED_CLAY.getState();

            if (previousPos + offset < 63 && rng.nextBoolean())
                this->clayBands[previousPos + offset + 1] = lce::BlocksInit::LIGHT_GRAY_HARDENED_CLAY.getState();
        }
    }
}

lce::BlockState BiomeMesa::getClayBand(c_int x, c_int y) {
    c_double noiseFactor = x / 512.0;
    c_int offset = static_cast<int>(round(this->clayBandsOffsetNoise.getValue(noiseFactor, noiseFactor) * 2.0));
    return this->clayBands[(y + offset + 64) % 64];
}

void BiomeSavannaMutated::genTerrainBlocks(MU i64 worldSeed, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x, c_int z,
                                           c_double noiseVal) {
    if (noiseVal > 1.75) {
        this->topBlock = lce::BlocksInit::STONE.getState();
        this->fillerBlock = lce::BlocksInit::STONE.getState();
    } else if (noiseVal > -0.5)
        this->topBlock = lce::BlocksInit::COARSE_DIRT.getState();

    this->generateBiomeTerrain(rng, chunkPrimerIn, x, z, noiseVal);
}

#pragma endregion

#pragma region Decorator

void Biome::decorate(World *worldIn, RNG &rng, const Pos2D &pos) {
    this->decorator->decorate(worldIn, this, rng, pos);
}

inline uint32_t Biome::getWaterColor() const {
    return this->waterColor;
}

inline float Biome::getRainfall() const {
    return this->rainFall;
}

void BiomePlains::decorate(World *worldIn, RNG &rng, const Pos2D &pos) {
    c_double d0 = INFO_NOISE.getValue(
        static_cast<double>(pos.x + 8) / 200.0,
        static_cast<double>(pos.z + 8) / 200.0);

    const Pos3D blockPos = {pos.x, 0, pos.z};
    if (d0 < -0.8) {
        this->decorator->flowersPerChunk = 15;
        this->decorator->grassPerChunk = 5;
    } else {
        this->decorator->flowersPerChunk = 4;
        this->decorator->grassPerChunk = 10;
        DOUBLE_PLANT_GENERATOR.setPlantType(BlockDoublePlant::EnumPlantType::GRASS);
        for (int i = 0; i < 7; ++i) {
            const int x_off = rng.nextInt(16) + 8;
            const int z_off = rng.nextInt(16) + 8;
            const int y_off = rng.nextInt(worldIn->getHeight(blockPos.add(x_off, 0, z_off)).getY() + 32);
            DOUBLE_PLANT_GENERATOR.generate(worldIn, rng, blockPos.add(x_off, y_off, z_off));
        }
    }

    if (this->hasSunflowers) {
        DOUBLE_PLANT_GENERATOR.setPlantType(BlockDoublePlant::EnumPlantType::SUNFLOWER);

        for (int i1 = 0; i1 < 10; ++i1) {
            const int x_off = rng.nextInt(16) + 8;
            const int z_off = rng.nextInt(16) + 8;
            const int y_off = rng.nextInt(worldIn->getHeight(blockPos.add(x_off, 0, z_off)).getY() + 32);
            DOUBLE_PLANT_GENERATOR.generate(worldIn, rng, blockPos.add(x_off, y_off, z_off));
        }
    }

    Biome::decorate(worldIn, rng, pos);
}

void BiomeDesert::decorate(World *worldIn, RNG &rng, const Pos2D &pos) {
    Biome::decorate(worldIn, rng, pos);

    // const Pos3D blockPos = {pos.x, 0, pos.z};
    if (rng.nextInt(1000) == 0) {
        // const int x_off = rng.nextInt(16) + 8;
        // const int z_off = rng.nextInt(16) + 8;
        // const Pos3D position = worldIn->getHeight(blockPos.add(x_off, 0, z_off));
        // WorldGenDesertWells().generate(worldIn, rng, position);
    }

    if (rng.nextInt(64) == 0) {
        // WorldGenFossils().generate(worldIn, rng, blockPos);
    }
}

void BiomeHills::decorate(World *worldIn, RNG &rng, const Pos2D &pos) {
    Biome::decorate(worldIn, rng, pos);
    const int emeraldAmount = 3 + rng.nextInt(6);

    const Pos3D blockPos = {pos.x, 0, pos.z};
    for (int j = 0; j < emeraldAmount; ++j) {
        c_int x_off = rng.nextInt(16);
        c_int y_off = rng.nextInt(28) + 4;
        c_int z_off = rng.nextInt(16);
        const Pos3D generatePos = blockPos.add(x_off, y_off, z_off);

        if (worldIn->getBlockId(generatePos) == lce::blocks::STONE_ID) {
            worldIn->setBlockId(generatePos, lce::blocks::EMERALD_ORE_ID);
        }
    }

    for (int j1 = 0; j1 < 7; ++j1) {
        c_int x_off = rng.nextInt(16);
        c_int y_off = rng.nextInt(64);
        c_int z_off = rng.nextInt(16);
        this->sliverfishGenerator->generate(worldIn, rng, blockPos.add(x_off, y_off, z_off));
    }
}

void BiomeForest::decorate(World *worldIn, RNG &rng, const Pos2D &pos) {
    if (this->type == Type::ROOFED) {
        addMushrooms(worldIn, rng, pos);
    }

    int i = rng.nextInt(5) - 3;

    if (this->type == Type::FLOWER) {
        i += 2;
    }

    addDoublePlants(worldIn, rng, pos, i);

    Biome::decorate(worldIn, rng, pos);
}

void BiomeForest::addMushrooms(World *worldIn, RNG &rng, const Pos2D &pos) const {
    const Pos3D blockPos = {pos.x, 0, pos.z};
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            c_int x_off = i * 4 + 1 + 8 + rng.nextInt(3);
            c_int z_off = j * 4 + 1 + 8 + rng.nextInt(3);
            Pos3D heightBlock = worldIn->getHeight(blockPos.add(x_off, 0, z_off));

            if (rng.nextInt(20) == 0) {
                WorldGenBigMushroom bigMushroom;
                bigMushroom.generate(worldIn, rng, heightBlock);
            } else {
                const WorldGenAbstractTree *abstractTree = this->genBigTreeChance(rng);
                abstractTree->setDecorationDefaults();

                if (abstractTree->generate(worldIn, rng, heightBlock)) {
                    abstractTree->generateSaplings(worldIn, rng, heightBlock);
                }
            }
        }
    }
}

void BiomeForest::addDoublePlants(World *worldIn, RNG &rng, const Pos2D &pos, const int amount) {
    const Pos3D blockPos = {pos.x, 0, pos.z};
    for (int i = 0; i < amount; ++i) {
        const int flowerType = rng.nextInt(3);

        if (flowerType == 0) {
            DOUBLE_PLANT_GENERATOR.setPlantType(BlockDoublePlant::EnumPlantType::SYRINGA);
        } else if (flowerType == 1) {
            DOUBLE_PLANT_GENERATOR.setPlantType(BlockDoublePlant::EnumPlantType::ROSE);
        } else if (flowerType == 2) {
            DOUBLE_PLANT_GENERATOR.setPlantType(BlockDoublePlant::EnumPlantType::PAEONIA);
        }

        for (int k = 0; k < 5; ++k) {
            const int x_off = rng.nextInt(16) + 8;
            const int z_off = rng.nextInt(16) + 8;
            const int y_off = rng.nextInt(worldIn->getHeight(blockPos.add(x_off, 0, z_off)).getY() + 32);
            if (DOUBLE_PLANT_GENERATOR.generate(worldIn, rng,
                                                {blockPos.getX() + x_off, y_off, blockPos.getZ() + z_off})) {
                break;
            }
        }
    }
}

void BiomeTaiga::decorate(World *worldIn, RNG &rng, const Pos2D &pos) {
    const Pos3D blockPos = {pos.x, 0, pos.z};
    if (this->type == Type::MEGA || this->type == Type::MEGA_SPRUCE) {
        const int i = rng.nextInt(3);

        for (int j = 0; j < i; ++j) {
            c_int x_off = rng.nextInt(16) + 8;
            c_int z_off = rng.nextInt(16) + 8;
            const Pos3D generatePos = worldIn->getHeight(blockPos.add(x_off, 0, z_off));
            FOREST_ROCK_GENERATOR.generate(worldIn, rng, generatePos);
        }
    }

    DOUBLE_PLANT_GENERATOR.setPlantType(BlockDoublePlant::EnumPlantType::FERN);

    for (int i1 = 0; i1 < 7; ++i1) {
        c_int x_off = rng.nextInt(16) + 8;
        c_int z_off = rng.nextInt(16) + 8;
        c_int y_off = rng.nextInt(worldIn->getHeight(blockPos.add(x_off, 0, z_off)).getY() + 32);
        DOUBLE_PLANT_GENERATOR.generate(worldIn, rng, blockPos.add(x_off, y_off, z_off));
    }

    Biome::decorate(worldIn, rng, pos);
}

void BiomeSwamp::decorate(World *worldIn, RNG &rng, const Pos2D &pos) {
    Biome::decorate(worldIn, rng, pos);

    if (rng.nextInt(64) == 0) {
        // const Pos3D blockPos = {pos.x, 0, pos.z};
        // (WorldGenFossils()).generate(worldIn, rng, blockPos);
    }
}

void BiomeSnow::decorate(World *worldIn, RNG &rng, const Pos2D &pos) {
    if (this->superIcy) {
        const Pos3D blockPos = {pos.x, 0, pos.z};
        for (int i = 0; i < 3; ++i) {
            const int x_off = rng.nextInt(16) + 8;
            const int z_off = rng.nextInt(16) + 8;
            this->iceSpike.generate(worldIn, rng, worldIn->getHeight(blockPos.add(x_off, 0, z_off)));
        }

        for (int l = 0; l < 2; ++l) {
            const int x_off = rng.nextInt(16) + 8;
            const int z_off = rng.nextInt(16) + 8;
            this->icePatch.generate(worldIn, rng, worldIn->getHeight(blockPos.add(x_off, 0, z_off)));
        }
    }

    Biome::decorate(worldIn, rng, pos);
}

void BiomeJungle::decorate(World *worldIn, RNG &rng, const Pos2D &pos) {
    Biome::decorate(worldIn, rng, pos);

    const Pos3D blockPos = {pos.x, 0, pos.z};
    int x_off = rng.nextInt(16) + 8;
    int z_off = rng.nextInt(16) + 8;
    const int y_off = rng.nextInt(worldIn->getHeight(blockPos.add(x_off, 0, z_off)).getY() * 2);
    /*if (pos.x >> 4 == 0 && pos.z >> 4 == 10) {
        std::cout << "Generating melon at " << x_off << ", " << y_off << ", " << z_off << std::endl;
        std::cout << rng.nextLong() << std::endl;
    }*/
    WorldGenCucurbitsBlock(lce::blocks::MELON_BLOCK_ID, false).generate(
        worldIn, rng, blockPos.add(x_off, y_off, z_off));
    const WorldGenVines genVines;

    for (int j1 = 0; j1 < 50; ++j1) {
        x_off = rng.nextInt(16) + 8;
        z_off = rng.nextInt(16) + 8;
        genVines.generate(worldIn, rng, blockPos.add(x_off, 128, z_off));
    }
}

void BiomeSavanna::decorate(World *worldIn, RNG &rng, const Pos2D &pos) {
    DOUBLE_PLANT_GENERATOR.setPlantType(BlockDoublePlant::EnumPlantType::GRASS);

    const Pos3D blockPos = {pos.x, 0, pos.z};
    for (int i = 0; i < 7; ++i) {
        c_int x_off = rng.nextInt(16) + 8;
        c_int z_off = rng.nextInt(16) + 8;
        c_int y_off = rng.nextInt(worldIn->getHeight(blockPos.add(x_off, 0, z_off)).getY() + 32);
        DOUBLE_PLANT_GENERATOR.generate(worldIn, rng, blockPos.add(x_off, y_off, z_off));
    }

    Biome::decorate(worldIn, rng, pos);
}

void BiomeSavannaMutated::decorate(World *worldIn, RNG &rng, const Pos2D &pos) {
    // TODO: was originally "Biome::decorate", but corrected to "BiomeSavanna::decorate". Is this Okay?
    BiomeSavanna::decorate(worldIn, rng, pos);
}

#pragma endregion

#pragma region TreeType

const WorldGenAbstractTree *BiomePlains::genBigTreeChance(RNG &rng) const {
    return rng.nextInt(3) == 0
               ? reinterpret_cast<const WorldGenAbstractTree *>(&BIG_TREE_FEATURE)
               : reinterpret_cast<const WorldGenAbstractTree *>(&TREE_FEATURE);
}

const WorldGenAbstractTree *BiomeHills::genBigTreeChance(RNG &rng) const {
    return rng.nextInt(3) > 0 ? this->spruceGenerator : Biome::genBigTreeChance(rng);
}

const WorldGenAbstractTree *BiomeForest::genBigTreeChance(RNG &rng) const {
    if (this->type == Type::ROOFED && 0 < rng.nextInt(3)) {
        return &ROOF_TREE;
    }
    if (this->type != Type::BIRCH && rng.nextInt(5) != 0) {
        return rng.nextInt(10) == 0
                   ? reinterpret_cast<const WorldGenAbstractTree *>(&BIG_TREE_FEATURE)
                   : reinterpret_cast<const WorldGenAbstractTree *>(&TREE_FEATURE);
    }
    return &BIRCH_TREE;
}

const WorldGenAbstractTree *BiomeForestMutated::genBigTreeChance(RNG &rng) const {
    return rng.nextBoolean() ? &BiomeForest::SUPER_BIRCH_TREE : &BiomeForest::BIRCH_TREE;
}

const WorldGenAbstractTree *BiomeTaiga::genBigTreeChance(RNG &rng) const {
    if ((this->type == Type::MEGA || this->type == Type::MEGA_SPRUCE) && rng.nextInt(3) == 0) {
        return this->type != Type::MEGA_SPRUCE && rng.nextInt(13) != 0
                   ? &MEGA_PINE_GENERATOR
                   : &MEGA_SPRUCE_GENERATOR;
    }

    return rng.nextInt(3) == 0
               ? reinterpret_cast<const WorldGenAbstractTree *>(&PINE_GENERATOR)
               : reinterpret_cast<const WorldGenAbstractTree *>(&SPRUCE_GENERATOR);
}

const WorldGenAbstractTree *BiomeSwamp::genBigTreeChance(MU RNG &rng) const {
    return &SWAMP_FEATURE;
}

const WorldGenAbstractTree *BiomeSnow::genBigTreeChance(MU RNG &rng) const {
    return &BiomeTaiga::SPRUCE_GENERATOR;
}

const WorldGenAbstractTree *BiomeJungle::genBigTreeChance(MU RNG &rng) const {
    if (rng.nextInt(10) == 0) {
        return &BIG_TREE_FEATURE;
    }
    if (rng.nextInt(2) == 0) {
        return new WorldGenShrub(lce::BlocksInit::JUNGLE_WOOD.getState(), lce::BlocksInit::OAK_LEAVES.getState());
    }

    return !this->isEdge && rng.nextInt(3) == 0
               ? reinterpret_cast<const WorldGenAbstractTree *>(&MEGA_JUNGLE_FEATURE)
               : reinterpret_cast<const WorldGenAbstractTree *>(&JUNGLE_TREES
                   [rng.nextInt(JUNGLE_TREE_HEIGHT_VARIATION)]);
}

const WorldGenAbstractTree *BiomeSavanna::genBigTreeChance(RNG &rng) const {
    return rng.nextInt(5) > 0
               ? reinterpret_cast<const WorldGenAbstractTree *>(&SAVANNA_TREE)
               : reinterpret_cast<const WorldGenAbstractTree *>(&TREE_FEATURE);
}

const WorldGenAbstractTree *BiomeMesa::genBigTreeChance(MU RNG &rng) const {
    return &TREE_FEATURE;
}

#pragma endregion

#pragma region GrassType

const AbstractWorldGenerator *BiomeTaiga::getRandomWorldGenForGrass(RNG &rng) const {
    return rng.nextInt(5) > 0 ? &FERN_GENERATOR : &GRASS_GENERATOR;
}

const AbstractWorldGenerator *BiomeJungle::getRandomWorldGenForGrass(RNG &rng) const {
    return rng.nextInt(4) == 0 ? &BiomeTaiga::FERN_GENERATOR : &BiomeTaiga::GRASS_GENERATOR;
}

#pragma endregion

#pragma region FlowerType

BlockFlower::EnumFlowerType BiomePlains::pickRandomFlower(RNG &rng, const Pos2D &pos) const {
    const double d0 = INFO_NOISE.
            getValue(static_cast<double>(pos.x) / 200.0, static_cast<double>(pos.z) / 200.0);

    if (d0 < -0.8) {
        const int i = rng.nextInt(4);
        switch (i) {
            case 0:
                return BlockFlower::EnumFlowerType::ORANGE_TULIP;
            case 1:
                return BlockFlower::EnumFlowerType::RED_TULIP;
            case 2:
                return BlockFlower::EnumFlowerType::PINK_TULIP;
            case 3:
            default:
                return BlockFlower::EnumFlowerType::WHITE_TULIP;
        }
    }
    if (rng.nextInt(3) > 0) {
        switch (rng.nextInt(3)) {
            case 0:
                return BlockFlower::EnumFlowerType::POPPY;
            case 1:
                return BlockFlower::EnumFlowerType::AZURE_BLUET;
            case 2:
            default:
                return BlockFlower::EnumFlowerType::OXEYE_DAISY;
        }
    }

    return BlockFlower::EnumFlowerType::DANDELION;
}

BlockFlower::EnumFlowerType BiomeForest::pickRandomFlower(RNG &rng, const Pos2D &pos) const {
    if (this->type != Type::FLOWER) {
        return Biome::pickRandomFlower(rng, pos);
    }

    const double d0 = std::clamp(
        (1.0 + INFO_NOISE.getValue(static_cast<double>(pos.x) / 48.0, static_cast<double>(pos.z) / 48.0)) / 2.0,
        0.0, 0.9999);

    const auto flowerType = static_cast<BlockFlower::EnumFlowerType>(
        static_cast<int>(d0 * static_cast<double>(BlockFlower::EnumFlowerType::NUM_TYPES)));

    return flowerType == BlockFlower::EnumFlowerType::BLUE_ORCHID ? BlockFlower::EnumFlowerType::POPPY : flowerType;
}


BlockFlower::EnumFlowerType BiomeSwamp::pickRandomFlower(MU RNG &rng, MU const Pos2D &pos) const {
    return BlockFlower::EnumFlowerType::BLUE_ORCHID;
}

#pragma endregion

#pragma region CustomDecorators

BiomeDecorator *BiomeMesa::createBiomeDecorator() const {
    return new Decorator();
}

void BiomeMesa::Decorator::generateOres(World *world, RNG &rng) {
    BiomeDecorator::generateOres(world, rng);
    this->genStandardOre1(world, rng, this->goldGen, 20, 32, 80);
}

#pragma endregion

#pragma region BiomeColors

uint32_t Biome::getGrassColor(const Pos3D &pos) const {
    const double _temperature = std::clamp(getFloatTemperature(pos), 0.0F, 1.0F);
    const double _rainFall = std::clamp(getRainfall(), 0.0F, 1.0F);
    return Colors::getGrassColor(_temperature, _rainFall);
}

MU uint32_t Biome::getFoliageColor(const Pos3D &pos) const {
    const double _temperature = std::clamp(getFloatTemperature(pos), 0.0F, 1.0F);
    const double _rainFall = std::clamp(getRainfall(), 0.0F, 1.0F);
    return Colors::getFoliageColor(_temperature, _rainFall);
}

uint32_t BiomeForest::getGrassColor(const Pos3D &pos) const {
    const u32 color = Biome::getGrassColor(pos);
    if (this->type != Type::ROOFED)
        return color;

    //2634762
    return (((color & 0xFEFEFE) + 0xA3428) >> 1) | 0xFF000000;
}

uint32_t BiomeSwamp::getGrassColor(const Pos3D &pos) const {
    const double d0 = INFO_NOISE.getValue(static_cast<double>(pos.x) * 0.0225, static_cast<double>(pos.z) * 0.0225);
    return d0 < -0.1 ? 0xFF3C764C : 0xFF39706A;
}

uint32_t BiomeSwamp::getFoliageColor(const Pos3D &pos) const {
    return 0xFF39706A;
}

uint32_t BiomeMesa::getFoliageColor(const Pos3D &pos) const {
    // return 10387789;
    return 0xFF4D819E;
}

uint32_t BiomeMesa::getGrassColor(const Pos3D &pos) const {
    // return 9470285;
    return 0xFF4D8190;
}


#pragma endregion
