#pragma once

#include <map>
#include <string>
#include <utility>

#include "NoiseGenerator.hpp"
#include "block.hpp"

#include "LegacyCubiomes/utils/rng.hpp"

#include "lce/blocks/block_ids.hpp"
#include "lce/blocks/blocks.hpp"

// #include "lce/items/item_ids.hpp"
// #include "lce/items/items.hpp"

#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/constants.hpp"

class ChunkPrimer;

class Biome {
public:
    static std::map<int, Biome*> registry;
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
    Block topBlock = Block(lce::blocks::ids::GRASS_ID, 0);

    /// The block to fill spots in when not on the top
    Block fillerBlock = Block(lce::blocks::ids::DIRT_ID, 0);

    NoiseGeneratorPerlin TEMPERATURE_NOISE;

    static Biome* getBiomeForId(c_int id) { return registry.at(id); }

    static void registerBiome(int id, Biome* biome) { registry.emplace(id, biome); }

    Biome(std::string biomeNameIn, c_float baseHeightIn, c_float heightVariationIn, c_bool enableSnowIn,
          c_float temperatureIn)
        : biomeName(std::move(biomeNameIn)), baseHeight(baseHeightIn), heightVariation(heightVariationIn),
          temperature(temperatureIn), enableSnow(enableSnowIn) {
        setTemperatureNoise();
    }

    Biome(std::string biomeNameIn, c_float baseHeightIn, c_float heightVariationIn, c_bool enableSnowIn,
          c_float temperatureIn, const Block topBlockIn)
        : biomeName(std::move(biomeNameIn)), baseHeight(baseHeightIn), heightVariation(heightVariationIn),
          temperature(temperatureIn), enableSnow(enableSnowIn), topBlock(topBlockIn) {
        setTemperatureNoise();
    }

    Biome(std::string biomeNameIn, c_float baseHeightIn, c_float heightVariationIn, c_bool enableSnowIn,
          c_float temperatureIn, const Block topBlockIn, const Block fillerBlockIn)
        : biomeName(std::move(biomeNameIn)), baseHeight(baseHeightIn), heightVariation(heightVariationIn),
          temperature(temperatureIn), enableSnow(enableSnowIn), topBlock(topBlockIn), fillerBlock(fillerBlockIn) {
        setTemperatureNoise();
    }

    virtual ~Biome() = default;

    void setTemperatureNoise() {
        RNG rng{};
        rng.setSeed(1234);
        this->TEMPERATURE_NOISE.setNoiseGeneratorPerlin(rng, 1);
    }

    float getFloatTemperature(const Pos3D pos) {
        if (pos.getY() > 64) {
            c_auto f =
                    (float) (TEMPERATURE_NOISE.getValue((float) pos.getX() / 8.0F, (float) pos.getZ() / 8.0F) * 4.0);
            return this->temperature - (f + (float) pos.getY() - 64.0F) * 0.05F / 30.0F;
        }
        return this->temperature;
    }

    void generateBiomeTerrain(RNG& rng, ChunkPrimer* chunkPrimerIn, int x, int z, double noiseVal);

    virtual void genTerrainBlocks(i64 worldSeed, RNG& rng, ChunkPrimer* chunkPrimerIn, c_int x, c_int z,
                                  c_double noiseVal) {
        this->generateBiomeTerrain(rng, chunkPrimerIn, x, z, noiseVal);
    }

    static void registerBiomes();
};

class BiomeOcean final : public Biome {
public:
    BiomeOcean(std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
               c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {}
};

class BiomePlains final : public Biome {
public:
    BiomePlains(std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
                c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {}
};

class BiomeDesert final : public Biome {
public:
    BiomeDesert(std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
                c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, Block(lce::blocks::ids::SAND_ID),
                Block(lce::blocks::ids::SAND_ID)) {}
};

class BiomeHills final : public Biome {
public:
    BiomeHills(std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
               c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {}

    void genTerrainBlocks(i64 worldSeed, RNG& rng, ChunkPrimer* chunkPrimerIn, c_int x, c_int z,
                          c_double noiseVal) override {
        this->topBlock.block = lce::blocks::ids::GRASS_ID;
        this->fillerBlock.block = lce::blocks::ids::DIRT_ID;

        if ((noiseVal < -1.0 || noiseVal > 2.0) &&
            (this->biomeName == "Extreme_Hills_M" || this->biomeName == "Extreme_Hills_Plus_M")) {
            this->topBlock.block = lce::blocks::ids::GRAVEL_ID;
            this->fillerBlock.block = lce::blocks::ids::GRAVEL_ID;
        } else if (noiseVal > 1.0 &&
                   (this->biomeName == "Extreme_Hills_M" || this->biomeName == "Extreme_Hills_Plus_M" ||
                    this->biomeName == "Extreme_Hills")) {
            this->topBlock.block = lce::blocks::ids::STONE_ID;
            this->fillerBlock.block = lce::blocks::ids::STONE_ID;
        }

        generateBiomeTerrain(rng, chunkPrimerIn, x, z, noiseVal);
    }
};

class BiomeForest : public Biome {
public:
    BiomeForest(std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
                c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {}
};

class BiomeForestMutated final : public BiomeForest {
public:
    BiomeForestMutated(std::string biomeName, c_float baseHeight, c_float heightVariation,
                       c_bool enableSnow, c_float temperature)
        : BiomeForest(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {}
};

class BiomeTaiga final : public Biome {
public:
    int type;

    BiomeTaiga(std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
               c_float temperature, c_int type)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature), type(type) {}

    void genTerrainBlocks(i64 worldSeed, RNG& rng, ChunkPrimer* chunkPrimerIn, c_int x, c_int z,
                          c_double noiseVal) override {
        if (this->type == 1) {
            if (noiseVal > 1.75) {
                this->topBlock = Block(lce::items::COARSE_DIRT);
            } else if (noiseVal > -0.95) {
                this->topBlock = Block(lce::items::PODZOL);
            }
        }
        return this->generateBiomeTerrain(rng, chunkPrimerIn, x, z, noiseVal);
    }
};

class BiomeSwamp final : public Biome {
public:
    BiomeSwamp(std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
               c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {}
};

class BiomeRiver final : public Biome {
public:
    BiomeRiver(std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
               c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {}
};

class BiomeSnow final : public Biome {
public:
    bool superIcy;

    BiomeSnow(c_bool superIcy, std::string biomeName, c_float baseHeight, c_float heightVariation,
              c_bool enableSnow, c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {
        this->superIcy = superIcy;
        if (superIcy) this->topBlock = Block(lce::blocks::ids::SNOW_BLOCK_ID);
    }
};

class BiomeMushroomIsland final : public Biome {
public:
    BiomeMushroomIsland(std::string biomeName, c_float baseHeight, c_float heightVariation,
                        c_bool enableSnow, c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, Block(lce::blocks::ids::MYCELIUM_ID)) {
    }
};

class BiomeBeach final : public Biome {
public:
    BiomeBeach(std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
               c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, Block(lce::blocks::ids::SAND_ID),
                Block(lce::blocks::ids::SAND_ID)) {}
};

class BiomeJungle final : public Biome {
public:
    BiomeJungle(std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
                c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {}
};

class BiomeStoneBeach final : public Biome {
public:
    BiomeStoneBeach(std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
                    c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, Block(lce::blocks::ids::STONE_ID),
                Block(lce::blocks::ids::STONE_ID)) {}
};

class BiomeSavanna final : public Biome {
public:
    BiomeSavanna(std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
                 c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {}
};

class BiomeMesa final : public Biome {
public:
    i64 worldSeed;
    std::vector<Block> clayBands;
    NoiseGeneratorPerlin pillarNoise;
    NoiseGeneratorPerlin pillarRoofNoise;
    NoiseGeneratorPerlin clayBandsOffsetNoise;
    bool brycePillars = false;
    bool hasForest = false;

    BiomeMesa(c_bool hasBrycePillars, c_bool hasForest, std::string biomeName, c_float baseHeight,
              c_float heightVariation, c_bool enableSnow, c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature), worldSeed(0),
          pillarNoise(), pillarRoofNoise(), clayBandsOffsetNoise(), brycePillars(hasBrycePillars),
          hasForest(hasForest) {}

    void genTerrainBlocks(i64 worldSeedIn, RNG& rng, ChunkPrimer* chunkPrimerIn, int x, int z,
                          double noiseVal) override;

    void generateClayBands(c_i64 seed) {
        this->clayBands.resize(64, Block(lce::blocks::ids::HARDENED_CLAY_ID));
        RNG rng;
        rng.setSeed(seed);
        this->clayBandsOffsetNoise.setNoiseGeneratorPerlin(rng, 1);

        for (int index = 0; index < 64; ++index) {
            index += rng.nextInt(5) + 1;

            if (index < 64) this->clayBands[index] = Block(lce::items::WHITE_HARDENED_CLAY);
        }

        c_int orangeBands = rng.nextInt(4) + 2;

        for (int i = 0; i < orangeBands; ++i) {
            c_int bandLength = rng.nextInt(3) + 1;
            c_int startPos = rng.nextInt(64);

            for (int offset = 0; startPos + offset < 64 && offset < bandLength; ++offset)
                this->clayBands[startPos + offset] = Block(lce::items::YELLOW_HARDENED_CLAY);
        }

        c_int brownBands = rng.nextInt(4) + 2;

        for (int i = 0; i < brownBands; ++i) {
            c_int bandLength = rng.nextInt(3) + 2;
            c_int startPos = rng.nextInt(64);

            for (int offset = 0; startPos + offset < 64 && offset < bandLength; ++offset)
                this->clayBands[startPos + offset] = Block(lce::items::BROWN_HARDENED_CLAY);
        }

        c_int redBands = rng.nextInt(4) + 2;

        for (int i = 0; i < redBands; ++i) {
            c_int bandLength = rng.nextInt(3) + 1;
            c_int startPos = rng.nextInt(64);

            for (int offset = 0; startPos + offset < 64 && offset < bandLength; ++offset)
                this->clayBands[startPos + offset] = Block(lce::items::RED_HARDENED_CLAY);
        }

        c_int grayBands = rng.nextInt(3) + 3;
        int previousPos = 0;

        for (int i = 0; i < grayBands; ++i) {
            previousPos += rng.nextInt(16) + 4;

            for (int offset = 0; previousPos + offset < 64 && offset < 1; ++offset) {
                this->clayBands[previousPos + offset] = Block(lce::items::WHITE_HARDENED_CLAY);

                if (previousPos + offset > 1 && rng.nextBoolean())
                    this->clayBands[previousPos + offset - 1] = Block(lce::items::LIGHT_GRAY_HARDENED_CLAY);

                if (previousPos + offset < 63 && rng.nextBoolean())
                    this->clayBands[previousPos + offset + 1] = Block(lce::items::LIGHT_GRAY_HARDENED_CLAY);
            }
        }
    }

    Block getClayBand(c_int x, c_int y) {
        c_double noiseFactor = x / 512.0;
        c_int offset = static_cast<int>(round(this->clayBandsOffsetNoise.getValue(noiseFactor, noiseFactor) * 2.0));
        return this->clayBands[(y + offset + 64) % 64];
    }
};

class BiomeSavannaMutated final : public Biome {
public:
    BiomeSavannaMutated(std::string biomeName, c_float baseHeight, c_float heightVariation,
                        c_bool enableSnow, c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {}

    void genTerrainBlocks(i64 worldSeed, RNG& rng, ChunkPrimer* chunkPrimerIn, c_int x, c_int z,
                          c_double noiseVal) override {
        if (noiseVal > 1.75) {
            this->topBlock.block = lce::blocks::ids::STONE_ID;
            this->fillerBlock.block = lce::blocks::ids::STONE_ID;
        } else if (noiseVal > -0.5)
            this->topBlock = Block(lce::items::COARSE_DIRT);
        return generateBiomeTerrain(rng, chunkPrimerIn, x, z, noiseVal);
    }
};
