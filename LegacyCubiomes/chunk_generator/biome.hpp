#pragma once

#include <string>
#include <map>
#include <utility>

#include "block.hpp"
#include "NoiseGenerator.hpp"

#include "LegacyCubiomes/utils/rng.hpp"

#include "LegacyCubiomes/mc/items.hpp"
#include "LegacyCubiomes/mc/itemID.hpp"

#include "LegacyCubiomes/utils/constants.hpp"
#include "LegacyCubiomes/utils/Pos3D.hpp"

class ChunkPrimer;

class Biome {
public:
    static std::map<int, Biome *> registry;
    std::string biomeName;
    /// The base height of this biome. Default 0.1.
    float baseHeight;
    /// The variation from the base height of the biome. Default 0.2.
    float heightVariation;
    /// The temperature of the biome. Default 0.5.
    float temperature;
    /** Set to true if snow is enabled for this biome. */
    bool enableSnow;

    /// The block expected to be on the top of this biome
    Block topBlock = Block(Items::GRASS_ID, 0);

    /// The block to fill spots in when not on the top
    Block fillerBlock = Block(Items::DIRT_ID, 0);
    NoiseGeneratorPerlin TEMPERATURE_NOISE;

    static Biome *getBiomeForId(int id) {
        return registry.at(id);
    }

    static void registerBiome(int id, Biome *biome) {
        registry.emplace(id, biome);
    }

    Biome(std::string biomeNameIn, float baseHeightIn, float heightVariationIn, bool enableSnowIn, float temperatureIn)
            : biomeName(std::move(biomeNameIn)), baseHeight(baseHeightIn), heightVariation(heightVariationIn),
              temperature(temperatureIn), enableSnow(enableSnowIn) {
        setTemperatureNoise();
    }

    Biome(std::string biomeNameIn, float baseHeightIn, float heightVariationIn, bool enableSnowIn, float temperatureIn,
          Block topBlockIn) : biomeName(std::move(biomeNameIn)), baseHeight(baseHeightIn), heightVariation(heightVariationIn),
                              temperature(temperatureIn), enableSnow(enableSnowIn), topBlock(topBlockIn) {
        setTemperatureNoise();
    }

    Biome(std::string biomeNameIn, float baseHeightIn, float heightVariationIn, bool enableSnowIn, float temperatureIn,
          Block topBlockIn, Block fillerBlockIn) : biomeName(std::move(biomeNameIn)), baseHeight(baseHeightIn),
                                                   heightVariation(heightVariationIn), temperature(temperatureIn),
                                                   enableSnow(enableSnowIn), topBlock(topBlockIn),
                                                   fillerBlock(fillerBlockIn) {
        setTemperatureNoise();
    }

    virtual ~Biome() = default;

    void setTemperatureNoise() {
        RNG rng{};
        rng.setSeed(1234);
        this->TEMPERATURE_NOISE.setNoiseGeneratorPerlin(rng, 1);
    }

    float getFloatTemperature(Pos3D pos) {
        if (pos.getY() > 64) {
            auto f = (float) (TEMPERATURE_NOISE.getValue((double) ((float) pos.getX() / 8.0F),
                                                         (double) ((float) pos.getZ() / 8.0F)) * 4.0);
            return this->temperature - (f + (float) pos.getY() - 64.0F) * 0.05F / 30.0F;
        } else
            return this->temperature;
    }

    void generateBiomeTerrain(RNG& rng, ChunkPrimer *chunkPrimerIn, int x, int z, double noiseVal);

    virtual void
    genTerrainBlocks(int64_t worldSeed, RNG& rng, ChunkPrimer *chunkPrimerIn, int x, int z, double noiseVal) {
        this->generateBiomeTerrain(rng, chunkPrimerIn, x, z, noiseVal);
    }

    static void registerBiomes();
};

class BiomeOcean : public Biome {
public:
    BiomeOcean(std::string biomeName, float baseHeight, float heightVariation, bool enableSnow, float temperature)
            : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {}
};

class BiomePlains : public Biome {
public:
    BiomePlains(std::string biomeName, float baseHeight, float heightVariation, bool enableSnow, float temperature)
            : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {}
};

class BiomeDesert : public Biome {
public:
    BiomeDesert(std::string biomeName, float baseHeight, float heightVariation, bool enableSnow, float temperature)
            : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, Block(Items::SAND_ID),
                    Block(Items::SAND_ID)) {}
};

class BiomeHills : public Biome {
public:
    BiomeHills(std::string biomeName, float baseHeight, float heightVariation, bool enableSnow, float temperature)
            : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {}

    void genTerrainBlocks(int64_t worldSeed, RNG& rng, ChunkPrimer *chunkPrimerIn, int x, int z,
                          double noiseVal) override {
        this->topBlock.block = Items::GRASS_ID;
        this->fillerBlock.block = Items::DIRT_ID;

        if ((noiseVal < -1.0 || noiseVal > 2.0) &&
            (this->biomeName == "Extreme_Hills_M" || this->biomeName == "Extreme_Hills_Plus_M")) {
            this->topBlock.block = Items::GRAVEL_ID;
            this->fillerBlock.block = Items::GRAVEL_ID;
        } else if (noiseVal > 1.0 &&
                   (this->biomeName == "Extreme_Hills_M" || this->biomeName == "Extreme_Hills_Plus_M" ||
                    this->biomeName == "Extreme_Hills")) {
            this->topBlock.block = Items::STONE_ID;
            this->fillerBlock.block = Items::STONE_ID;
        }

        generateBiomeTerrain(rng, chunkPrimerIn, x, z, noiseVal);
    }
};

class BiomeForest : public Biome {
public:
    BiomeForest(std::string biomeName, float baseHeight, float heightVariation, bool enableSnow, float temperature)
            : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {}
};

class BiomeForestMutated : public BiomeForest {
public:
    BiomeForestMutated(std::string biomeName, float baseHeight, float heightVariation, bool enableSnow,
                       float temperature)
            : BiomeForest(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {}
};

class BiomeTaiga : public Biome {
public:
    int type;

    BiomeTaiga(std::string biomeName, float baseHeight, float heightVariation, bool enableSnow, float temperature,
               int type)
            : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature), type(type) {}

    void genTerrainBlocks(int64_t worldSeed, RNG& rng, ChunkPrimer *chunkPrimerIn, int x, int z,
                          double noiseVal) override {
        if (this->type == 1) {
            if (noiseVal > 1.75) {
                this->topBlock = Block(Items::COARSE_DIRT);
            } else if (noiseVal > -0.95) {
                this->topBlock = Block(Items::PODZOL);
            }
        }
        return this->generateBiomeTerrain(rng, chunkPrimerIn, x, z, noiseVal);
    }
};

class BiomeSwamp : public Biome {
public:
    BiomeSwamp(std::string biomeName, float baseHeight, float heightVariation, bool enableSnow, float temperature)
            : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {}
};

class BiomeRiver : public Biome {
public:
    BiomeRiver(std::string biomeName, float baseHeight, float heightVariation, bool enableSnow, float temperature)
            : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {}
};

class BiomeSnow : public Biome {
public:
    bool superIcy;

    BiomeSnow(bool superIcy, std::string biomeName, float baseHeight, float heightVariation, bool enableSnow,
              float temperature)
            : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {
        this->superIcy = superIcy;
        if (superIcy)
            this->topBlock = Block(Items::SNOW_BLOCK_ID);
    }
};

class BiomeMushroomIsland : public Biome {
public:
    BiomeMushroomIsland(std::string biomeName, float baseHeight, float heightVariation, bool enableSnow,
                        float temperature)
            : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, Block(Items::MYCELIUM_ID)) {}
};

class BiomeBeach : public Biome {
public:
    BiomeBeach(std::string biomeName, float baseHeight, float heightVariation, bool enableSnow, float temperature)
            : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, Block(Items::SAND_ID),
                    Block(Items::SAND_ID)) {}
};

class BiomeJungle : public Biome {
public:
    BiomeJungle(std::string biomeName, float baseHeight, float heightVariation, bool enableSnow, float temperature)
            : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {}
};

class BiomeStoneBeach : public Biome {
public:
    BiomeStoneBeach(std::string biomeName, float baseHeight, float heightVariation, bool enableSnow, float temperature)
            : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, Block(Items::STONE_ID),
                    Block(Items::STONE_ID)) {}
};

class BiomeSavanna : public Biome {
public:
    BiomeSavanna(std::string biomeName, float baseHeight, float heightVariation, bool enableSnow, float temperature)
            : Biome(biomeName, baseHeight, heightVariation, enableSnow, temperature) {}
};

class BiomeMesa : public Biome {
public:
    int64_t worldSeed;
    std::vector<Block> clayBands;
    NoiseGeneratorPerlin pillarNoise;
    NoiseGeneratorPerlin pillarRoofNoise;
    NoiseGeneratorPerlin clayBandsOffsetNoise;
    bool brycePillars = false;
    bool hasForest = false;

    BiomeMesa(bool hasBrycePillars, bool hasForest, std::string biomeName, float baseHeight, float heightVariation,
              bool enableSnow, float temperature)
            : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature), worldSeed(0),
              brycePillars(hasBrycePillars), hasForest(hasForest) {}

    void genTerrainBlocks(int64_t worldSeed, RNG& rng, ChunkPrimer *chunkPrimerIn, int x, int z,
                          double noiseVal) override;

    void generateClayBands(int64_t seed) {
        this->clayBands.resize(64, Block(Items::HARDENED_CLAY_ID));
        RNG rng;
        rng.setSeed(seed);
        this->clayBandsOffsetNoise.setNoiseGeneratorPerlin(rng, 1);

        for (int index = 0; index < 64; ++index) {
            index += rng.nextInt(5) + 1;

            if (index < 64)
                this->clayBands[index] = Block(Items::WHITE_HARDENED_CLAY);
        }

        int orangeBands = rng.nextInt(4) + 2;

        for (int i = 0; i < orangeBands; ++i) {
            int bandLength = rng.nextInt(3) + 1;
            int startPos = rng.nextInt(64);

            for (int offset = 0; startPos + offset < 64 && offset < bandLength; ++offset)
                this->clayBands[startPos + offset] = Block(Items::YELLOW_HARDENED_CLAY);
        }

        int brownBands = rng.nextInt(4) + 2;

        for (int i = 0; i < brownBands; ++i) {
            int bandLength = rng.nextInt(3) + 2;
            int startPos = rng.nextInt(64);

            for (int offset = 0; startPos + offset < 64 && offset < bandLength; ++offset)
                this->clayBands[startPos + offset] = Block(Items::BROWN_HARDENED_CLAY);
        }

        int redBands = rng.nextInt(4) + 2;

        for (int i = 0; i < redBands; ++i) {
            int bandLength = rng.nextInt(3) + 1;
            int startPos = rng.nextInt(64);

            for (int offset = 0; startPos + offset < 64 && offset < bandLength; ++offset)
                this->clayBands[startPos + offset] = Block(Items::RED_HARDENED_CLAY);
        }

        int grayBands = rng.nextInt(3) + 3;
        int previousPos = 0;

        for (int i = 0; i < grayBands; ++i) {
            previousPos += rng.nextInt(16) + 4;

            for (int offset = 0; previousPos + offset < 64 && offset < 1; ++offset) {
                this->clayBands[previousPos + offset] = Block(Items::WHITE_HARDENED_CLAY);

                if (previousPos + offset > 1 && rng.nextBoolean())
                    this->clayBands[previousPos + offset - 1] = Block(Items::LIGHT_GRAY_HARDENED_CLAY);

                if (previousPos + offset < 63 && rng.nextBoolean())
                    this->clayBands[previousPos + offset + 1] = Block(Items::LIGHT_GRAY_HARDENED_CLAY);
            }
        }
    }

    Block getClayBand(int x, int y) {
        double noiseFactor = x / 512.0;
        int offset = (int) round(this->clayBandsOffsetNoise.getValue(noiseFactor, noiseFactor) * 2.0);
        return this->clayBands[(y + offset + 64) % 64];
    }

};

class BiomeSavannaMutated : public Biome {
public:
    BiomeSavannaMutated(std::string biomeName, float baseHeight, float heightVariation, bool enableSnow,
                        float temperature)
            : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {}

    void genTerrainBlocks(int64_t worldSeed, RNG& rng, ChunkPrimer *chunkPrimerIn, int x, int z,
                          double noiseVal) override {
        if (noiseVal > 1.75) {
            this->topBlock.block = Items::STONE_ID;
            this->fillerBlock.block = Items::STONE_ID;
        } else if (noiseVal > -0.5)
            this->topBlock = Block(Items::COARSE_DIRT);
        return generateBiomeTerrain(rng, chunkPrimerIn, x, z, noiseVal);
    }
};
