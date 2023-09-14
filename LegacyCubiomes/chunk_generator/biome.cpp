
#include "biome.hpp"
#include "ChunkPrimer.hpp"

std::map<int, Biome*> Biome::registry;
void Biome::registerBiomes() {
    registerBiome(0, new BiomeOcean("Ocean", -1.0F, 0.1F, false, 0.5F));
    registerBiome(1, new BiomePlains("Plains", 0.125F, 0.05F, false, 0.8F));
    registerBiome(2, new BiomeDesert("Desert", 0.125F, 0.05F, false, 2.0F));
    registerBiome(3, new BiomeHills("Extreme_Hills", 1.0, 0.5F, false, 0.2F));
    registerBiome(4, new BiomeForest("Forest", 0.1F, 0.2F, false, 0.2F));
    registerBiome(5, new BiomeTaiga("Taiga", 0.2F, 0.2F, false, 0.7F, 0));
    registerBiome(6, new BiomeSwamp("Swampland", -0.2, 0.1F, false, 0.25F));
    registerBiome(7, new BiomeRiver("River", -0.5F, 0.0F, false, 0.5F));
    registerBiome(8, new Biome("Nether", 0.1F, 0.2F, false, 2.0F));
    registerBiome(9, new Biome("The End", 0.1F, 0.2F, false, 0.5F));
    registerBiome(10, new BiomeOcean("FrozenOcean", -1.0F, 0.1F, true, 0.0F));
    registerBiome(11, new BiomeRiver("FrozenRiver", -0.5F, 0.0F, true, 0.0F));
    registerBiome(12, new BiomeSnow(false, "Ice_Plains", 0.125F, 0.05F, true, 0.0F));
    registerBiome(13, new BiomeSnow(false, "Ice_Mountains", 0.45F, 0.3F, true, 0.0F));
    registerBiome(14, new BiomeMushroomIsland("MushroomIsland", 0.2F, 0.3F, false, 0.9F));
    registerBiome(15, new BiomeMushroomIsland("MushroomIslandShore", 0.0F, 0.025F, false, 0.9F));
    registerBiome(16, new BiomeBeach("Beach", 0.0F, 0.025F, false, 0.8F));
    registerBiome(17, new BiomeDesert("DesertHills", 0.45, 0.3F, false, 2.0F));
    registerBiome(18, new BiomeForest("ForestHills", 0.45F, 0.3F, false, 0.7F));
    registerBiome(19, new BiomeTaiga("TaigaHills", 0.45F, 0.3F, false, 0.25F, 0));
    registerBiome(20, new BiomeHills("Extreme_Hills_Edge", 0.8F, 0.3F, false, 0.2F));
    registerBiome(21, new BiomeJungle("Jungle", 0.1F, 0.2F, false, 0.95F));
    registerBiome(22, new BiomeJungle("JungleHills", 0.45F, 0.3F, false, 0.95F));
    registerBiome(23, new BiomeJungle("JungleEdge", 0.1F, 0.2F, false, 0.95F));
    registerBiome(24, new BiomeOcean("Deep_Ocean", -1.8F, 0.1F, false, 0.5F));
    registerBiome(25, new BiomeStoneBeach("Stone_Beach", 0.1F, 0.8F, false, 0.2F));
    registerBiome(26, new BiomeBeach("Cold_Beach", 0.0F, 0.025F, true, 0.05F));
    registerBiome(27, new BiomeForest("Birch_Forest", 0.1F, 0.2F, false, 0.6F));
    registerBiome(28, new BiomeForest("Birch_Forest_Hills", 0.45F, 0.3F, false, 0.6F));
    registerBiome(29, new BiomeForest("Roofed_Forest", 0.1F, 0.2F, false, 0.7F));
    registerBiome(30, new BiomeTaiga("Cold_Taiga", 0.2F, 0.2F, true, -0.5F, 0));
    registerBiome(31, new BiomeTaiga("Cold_Taiga_Hills", 0.45F, 0.3F, true, -0.5F, 0));
    registerBiome(32, new BiomeTaiga("Mega_Taiga", 0.2F, 0.2F, false, 0.3F, 1));
    registerBiome(33, new BiomeTaiga("Mega_Taiga_Hills", 0.45F, 0.3F, false, 0.3F, 1));
    registerBiome(34, new BiomeHills("Extreme_Hills_Plus", 1.0F, 0.5F, false, 0.2F));
    registerBiome(35, new BiomeSavanna("Savanna", 0.125F, 0.05F, false, 1.2F));
    registerBiome(36, new BiomeSavanna("Savanna_Plateau", 1.5F, 0.025F, false, 1.0F));
    registerBiome(37, new BiomeMesa(false, false, "Mesa", 0.1F, 0.2F, false, 2.0F));
    registerBiome(38, new BiomeMesa(false, true, "Mesa_Plateau_F", 1.5F, 0.025F, false, 2.0F));
    registerBiome(39, new BiomeMesa(false, false, "Mesa_Plateau", 1.5F, 0.025F, false, 2.0F));
    //mutated
    registerBiome(129, new BiomePlains("Sunflower_Plains", 0.125F, 0.05F, false, 0.8F));
    registerBiome(130, new BiomeDesert("Desert_M", 0.225F, 0.25F, false, 2.0F));
    registerBiome(131, new BiomeHills("Extreme_Hills_M", 1.0F, 0.5F, false, 0.2F));
    registerBiome(132, new BiomeForest("Flower_Forest", 0.1F, 0.4F, false, 0.7F));
    registerBiome(133, new BiomeTaiga("Taiga_M", 0.3F, 0.4F, false, 0.25F, 0));
    registerBiome(134, new BiomeSwamp("Swampland_M", -0.1F, 0.3F, false, 0.8F));
    registerBiome(140, new BiomeSnow(true, "Ice_Plains_Spikes", 0.425F, 0.45000002F, true, 0.0F));
    registerBiome(149, new BiomeJungle("Jungle_M", 0.2F, 0.4F, false, 0.95F));
    registerBiome(151, new BiomeJungle("JungleEdge_M", 0.2F, 0.4F, false, 0.95F));
    registerBiome(155, new BiomeForestMutated("Birch_Forest_M", 0.2F, 0.4F, false, 0.6F));
    registerBiome(156, new BiomeForestMutated("Birch_Forest_Hills_M", 0.55F, 0.5F, false, 0.6F));
    registerBiome(157, new BiomeForest("Roofed_Forest_M", 0.2F, 0.4F, false, 0.7F));
    registerBiome(158, new BiomeTaiga("Cold_Taiga_M", 0.3F, 0.4F, true, -0.5F, 0));
    registerBiome(160, new BiomeTaiga("Mega_Spruce_Taiga", 0.2F, 0.2F, false, 0.25F, 1));
    registerBiome(161, new BiomeTaiga("Redwood_Taiga_Hills_M", 0.2F, 0.2F, false, 0.25F, 1));
    registerBiome(162, new BiomeHills("Extreme_Hills_Plus_M", 1.0F, 0.5F, false, 0.2F));
    registerBiome(163, new BiomeSavanna("Savanna_M", 0.3625F, 1.225F, false, 1.1F));
    registerBiome(164, new BiomeSavanna("Savanna_Plateau_M", 1.05F, 1.2125001F, false, 1.0F));
    registerBiome(165, new BiomeMesa(true, false, "Mesa_Bryce", 0.1F, 0.2F, false, 2.0F));
    registerBiome(166, new BiomeMesa(false, true, "Mesa_Plateau_F_M", 0.45F, 0.3F, false, 2.0F));
    registerBiome(167, new BiomeMesa(false, false, "Mesa_Plateau_M", 0.45F, 0.3F, false, 2.0F));
}

void Biome::generateBiomeTerrain(uint64_t* rand, ChunkPrimer* chunkPrimerIn, int x, int z, double noiseVal)
{
    int seaLevel = 63; // sea level
    uint16_t iBlockState = this->topBlock.block;
    uint16_t iBlockState1 = this->fillerBlock.block;
    int j = -1;
    int k = (int)(((double)noiseVal / 3.0) + 3.0 + nextDouble(rand) * 0.25);
    int l = x & 15;
    int i1 = z & 15;
    Pos3D blockPos;

    for (int j1 = 255; j1 >= 0; --j1)
    {
        if (j1 == 0)
            break;

        if (nextInt(rand, 2) + 1 >= j1 - 1)
            chunkPrimerIn->setBlock(i1, j1 - 1, l, Items::BEDROCK_ID);
        else
        {
            uint16_t iBlockState2 = chunkPrimerIn->getBlock(i1, j1, l);

            if (!iBlockState2) // if air
                j = -1;
            else if (iBlockState2 == Items::STONE_ID)
            {
                if (j == -1)
                {
                    if (k <= 0)
                    {
                        iBlockState = Items::AIR_ID;
                        iBlockState1 = Items::STONE_ID;
                    }
                    else if (j1 >= seaLevel - 4 && j1 <= seaLevel + 1)
                    {
                        iBlockState = this->topBlock.block;
                        iBlockState1 = this->fillerBlock.block;
                    }

                    if (j1 < seaLevel && !iBlockState)
                    {
                        blockPos.setPos(x, j1, z);
                        if (this->getFloatTemperature(blockPos) < 0.15F)
                            iBlockState = Items::ICE_ID;
                        else
                            iBlockState = Items::STILL_WATER_ID;
                    }

                    j = k;

                    if (j1 >= seaLevel - 1)
                        chunkPrimerIn->setBlock(i1, j1, l, iBlockState);
                    else if (j1 < seaLevel - 7 - k)
                    {
                        iBlockState = Items::AIR_ID;
                        iBlockState1 = Items::STONE_ID;
                        chunkPrimerIn->setBlock(i1, j1, l, Items::GRAVEL_ID);
                    }
                    else
                        chunkPrimerIn->setBlock(i1, j1, l, iBlockState1);
                }
                else if (j > 0)
                {
                    --j;
                    chunkPrimerIn->setBlock(i1, j1, l, iBlockState1);

                    if (j == 0 && iBlockState1 == Items::SAND_ID && k > 1)
                    {
                        j = nextInt(rand, 4) + std::max(0, j1 - 63);
                        iBlockState1 = Items::SANDSTONE_ID; //179 for redsandstone //((iblockstate1) == "red_sand" ? "red_sandstone" : "sandstone");
                    }
                }
            }
        }
    }
}

void BiomeMesa::genTerrainBlocks(int64_t worldSeed, uint64_t* rand, ChunkPrimer *chunkPrimerIn, int x, int z, double noiseVal)
{
    this->worldSeed = worldSeed;
    if (this->clayBands.empty())
    {
        this->generateClayBands(worldSeed);
    }

    if (this->pillarNoise.noiseLevels.empty() || this->pillarRoofNoise.noiseLevels.empty())
    {
        uint64_t random;
        setSeed(&random, worldSeed);
        this->pillarNoise.setNoiseGeneratorPerlin(&random, 4);
        this->pillarRoofNoise.setNoiseGeneratorPerlin(&random, 1);
    }
    double d4 = 0.0;

    if (this->brycePillars)
    {
        int i = (x & -16) + (z & 15);
        int j = (z & -16) + (x & 15);
        double d0 = std::min(std::abs(noiseVal), this->pillarNoise.getValue((double)i * 0.25, (double)j * 0.25));

        if (d0 > 0.0)
        {
            double d2 = std::abs(this->pillarRoofNoise.getValue((double)i * 0.001953125, (double)j * 0.001953125));
            d4 = d0 * d0 * 2.5;
            double d3 = ceil(d2 * 50.0) + 14.0;

            if (d4 > d3)
                d4 = d3;

            d4 = d4 + 64.0;
        }
    }

    int k1 = x & 15;
    int l1 = z & 15;
    int seaLevel = 63; // sea level
    uint16_t fillerBlock = this->fillerBlock.block;
    int k = (int)(noiseVal / 3.0 + 3.0 + nextDouble(rand) * 0.25);
    bool flag = cos(noiseVal / 3.0 * PI) > 0.0;
    int l = -1;
    bool flag1 = false;
    int i1 = 0;

    for (int j1 = 255; j1 >= 0; --j1)
    {
        if (!chunkPrimerIn->getBlock(l1, j1, k1) && j1 < (int)d4)
        {
            chunkPrimerIn->setBlock(l1, j1, k1, Items::STONE_ID);
        }
        if (j1 == 0)
            break;

        if (nextInt(rand, 2) + 1 >= j1 - 1)
            chunkPrimerIn->setBlock(l1, j1 - 1, k1, Items::BEDROCK_ID);
        else if (i1 < 15 || this->brycePillars)
        {
            uint16_t iBlockState1 = chunkPrimerIn->getBlock(l1, j1, k1);

            if (!iBlockState1)
                l = -1;
            else if (iBlockState1 == Items::STONE_ID)
            {
                if (l == -1)
                {
                    flag1 = false;

                    if (k <= 0)
                    {
                        fillerBlock = Items::STONE_ID;
                    }
                    else if (j1 >= seaLevel - 4 && j1 <= seaLevel + 1)
                    {
                        fillerBlock = this->fillerBlock.block;
                    }

                    l = k + std::max(0, j1 - seaLevel);

                    if (j1 >= seaLevel - 1)
                    {
                        if (this->hasForest && j1 > 86 + k * 2)
                        {
                            if (flag)
                                chunkPrimerIn->setBlockAndData(l1, j1, k1, Items::COARSE_DIRT);
                            else
                                chunkPrimerIn->setBlock(l1, j1, k1, Items::GRASS_ID);
                        }
                        else if (j1 > seaLevel + 3 + k)
                        {
                            uint16_t iBlockState2;

                            if (j1 >= 64 && j1 <= 127)
                            {
                                if (flag)
                                    iBlockState2 = Items::HARDENED_CLAY_ID;
                                else
                                    iBlockState2 = this->getClayBand(x, j1).block;
                            }
                            else
                                iBlockState2 = Items::WHITE_HARDENED_CLAY_ID;

                            chunkPrimerIn->setBlock(l1, j1, k1, iBlockState2);
                        }
                        else
                        {
                            chunkPrimerIn->setBlock(l1, j1, k1, this->topBlock.block);
                            flag1 = true;
                        }
                    }
                    else
                    {
                        chunkPrimerIn->setBlock(l1, j1, k1, fillerBlock);

                        if (fillerBlock == Items::WHITE_HARDENED_CLAY_ID)
                            chunkPrimerIn->setBlockAndData(l1, j1, k1, Items::ORANGE_HARDENED_CLAY);
                    }
                }
                else if (l > 0)
                {
                    --l;

                    if (flag1)
                        chunkPrimerIn->setBlockAndData(l1, j1, k1, Items::ORANGE_HARDENED_CLAY);
                    else
                        chunkPrimerIn->setBlock(l1, j1, k1, this->getClayBand(x, j1).block);
                }

                ++i1;
            }
        }
    }
}
