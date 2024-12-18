#pragma once

#include <map>
#include <string>
#include <utility>

#include "NoiseGenerator.hpp"

#include "LegacyCubiomes/features/BiomeDecorator.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

#include "lce/blocks/block_ids.hpp"
#include "lce/blocks/blocks.hpp"
#include "LegacyCubiomes/features/WorldGenerator/WorldGenAbstractTree.hpp"
#include "LegacyCubiomes/features/WorldGenerator/WorldGenIcePath.hpp"
#include "LegacyCubiomes/features/WorldGenerator/WorldGenIceSpike.hpp"
#include "LegacyCubiomes/features/WorldGenerator/WorldGenTaiga2.hpp"
#include "LegacyCubiomes/features/WorldGenerator/WorldGenFlowers.hpp"

#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/constants.hpp"

class WorldGenDoublePlant;
class WorldGenSavannaTree;
class WorldGenSwamp;
class WorldGenCanopyTree;
class WorldGenBirchTree;
class WorldGenTrees;
class WorldGenBigTree;
class BiomeDecorator;
class ChunkPrimer;

class Biome {
public:
    static const NoiseGeneratorPerlin TEMPERATURE_NOISE;
    static const NoiseGeneratorPerlin GRASS_COLOR_NOISE;
    static const WorldGenDoublePlant DOUBLE_PLANT_GENERATOR;

    /** The tree generator. */
    static const WorldGenTrees TREE_FEATURE;

    /** The big tree generator. */
    static const WorldGenBigTree BIG_TREE_FEATURE;

    static std::map<int, Biome *> registry;
    std::string biomeName;

    /// The base height of this biome. Default 0.1.
    MU float baseHeight;

    /// The variation from the base height of the biome. Default 0.2.
    MU float heightVariation;

    /// The temperature of the biome. Default 0.5.
    float temperature;

    /** Set to true if snow is enabled for this biome. */
    MU bool enableSnow;

    /// The block expected to be on the top of this biome
    lce::blocks::Block const *topBlock = &lce::blocks::GRASS;

    /// The block to fill spots in when not on the top
    lce::blocks::Block const *fillerBlock = &lce::blocks::DIRT;

    BiomeDecorator *decorator = this->createBiomeDecorator();

    static Biome *getBiomeForId(c_int id) { return registry.at(id); }

    static void registerBiome(int id, Biome *biome) { registry.emplace(id, biome); }

    Biome(std::string biomeNameIn, c_float baseHeightIn, c_float heightVariationIn, c_bool enableSnowIn,
          c_float temperatureIn)
        : biomeName(std::move(biomeNameIn)), baseHeight(baseHeightIn), heightVariation(heightVariationIn),
          temperature(temperatureIn), enableSnow(enableSnowIn) {
    }

    Biome(std::string biomeNameIn, c_float baseHeightIn, c_float heightVariationIn, c_bool enableSnowIn,
          c_float temperatureIn, lce::blocks::Block const *topBlockIn)
        : biomeName(std::move(biomeNameIn)), baseHeight(baseHeightIn), heightVariation(heightVariationIn),
          temperature(temperatureIn), enableSnow(enableSnowIn), topBlock(topBlockIn) {
    }

    Biome(std::string biomeNameIn, c_float baseHeightIn, c_float heightVariationIn, c_bool enableSnowIn,
          c_float temperatureIn, lce::blocks::Block const *topBlockIn, lce::blocks::Block const *fillerBlockIn)
        : biomeName(std::move(biomeNameIn)), baseHeight(baseHeightIn), heightVariation(heightVariationIn),
          temperature(temperatureIn), enableSnow(enableSnowIn), topBlock(topBlockIn), fillerBlock(fillerBlockIn) {
    }

    virtual ~Biome();

protected:
    ND virtual BiomeDecorator *createBiomeDecorator() const;

public:
    ND virtual const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const;

    ND virtual const WorldGenerator *getRandomWorldGenForGrass(RNG &rng) const;

    ND virtual BlockFlower::EnumFlowerType pickRandomFlower(RNG &rng, const Pos2D &pos) const;

    ND float getFloatTemperature(const Pos3D &pos) const;

    virtual void decorate(ChunkPrimer *worldIn, RNG &rand, const Pos2D &pos);

    void generateBiomeTerrain(RNG &rng, ChunkPrimer *chunkPrimerIn, int x, int z, double noiseVal);

    virtual void genTerrainBlocks(i64 worldSeed, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x, c_int z,
                                  c_double noiseVal);

    static void registerBiomes();
};

class BiomeOcean final : public Biome {
public:
    BiomeOcean(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
               c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {
    }
};

class BiomePlains final : public Biome {
public:
    bool hasSunflowers;

    BiomePlains(c_bool hasSunflowers, MU std::string biomeName, c_float baseHeight, c_float heightVariation,
                c_bool enableSnow,
                c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature),
          hasSunflowers(hasSunflowers) {
        this->decorator->treesPerChunk = 0;
        this->decorator->extraTreeChance = 0.05F;
        this->decorator->flowersPerChunk = 4;
        this->decorator->grassPerChunk = 10;
    }

    void decorate(ChunkPrimer *worldIn, RNG &rng, const Pos2D &pos) override;

    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;
};

class BiomeDesert final : public Biome {
public:
    BiomeDesert(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
                c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, &lce::blocks::SAND,
                &lce::blocks::SAND) {
        this->decorator->treesPerChunk = -999;
        this->decorator->deadBushPerChunk = 2;
        this->decorator->reedsPerChunk = 50;
        this->decorator->cactiPerChunk = 10;
    }

    void decorate(ChunkPrimer *worldIn, RNG &rng, const Pos2D &pos) override;
};

class BiomeHills final : public Biome {
public:
    enum class Type {
        NORMAL,
        EXTRA_TREES,
        MUTATED
    };

    BiomeHills(const BiomeHills::Type typeIn, MU std::string biomeName, c_float baseHeight, c_float heightVariation,
               c_bool enableSnow,
               c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature), type(typeIn) {
        if (type == BiomeHills::Type::EXTRA_TREES) {
            this->decorator->treesPerChunk = 3;
        }
    }

    ~BiomeHills() override {
        delete spruceGenerator;
        delete sliverfishGenerator;
    }

    void genTerrainBlocks(i64 worldSeed, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x, c_int z,
                          c_double noiseVal) override;

    void decorate(ChunkPrimer *worldIn, RNG &rng, const Pos2D &pos) override;

    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

private:
    WorldGenAbstractTree *spruceGenerator = new WorldGenTaiga2();
    WorldGenMinable *sliverfishGenerator = new WorldGenMinable(&lce::blocks::STONE_MONSTER_EGG, 9);
    BiomeHills::Type type;
};

class BiomeForest : public Biome {
public:
    enum class Type {
        NORMAL,
        FLOWER,
        BIRCH,
        ROOFED
    };

    BiomeForest(const BiomeForest::Type typeIn, MU std::string biomeName, c_float baseHeight, c_float heightVariation,
                c_bool enableSnow, c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature), type(typeIn) {
        this->decorator->treesPerChunk = 10;
        this->decorator->grassPerChunk = 2;

        if (type == BiomeForest::Type::FLOWER) {
            this->decorator->treesPerChunk = 6;
            this->decorator->flowersPerChunk = 100;
            this->decorator->grassPerChunk = 1;
        } else if (type == BiomeForest::Type::ROOFED) {
            this->decorator->treesPerChunk = -999;
        }
    }

    void decorate(ChunkPrimer *worldIn, RNG &rng, const Pos2D &pos) override;

    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

    ND BlockFlower::EnumFlowerType pickRandomFlower(RNG &rng, const Pos2D &pos) const override;

private:
    void addMushrooms(ChunkPrimer *worldIn, RNG &rng, const Pos2D &pos) const;

    static void addDoublePlants(ChunkPrimer *worldIn, RNG &rng, const Pos2D &pos, int amount);

    BiomeForest::Type type;

protected:
    static const WorldGenBirchTree SUPER_BIRCH_TREE;
    static const WorldGenBirchTree BIRCH_TREE;
    static const WorldGenCanopyTree ROOF_TREE;
};

class BiomeForestMutated final : public BiomeForest {
public:
    BiomeForestMutated(MU std::string biomeName, c_float baseHeight, c_float heightVariation,
                       c_bool enableSnow, c_float temperature)
        : BiomeForest(BiomeForest::Type::BIRCH, std::move(biomeName), baseHeight, heightVariation, enableSnow,
                      temperature) {
    }

    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;
};

class WorldGenTaiga1;
class WorldGenTaiga2;
class WorldGenMegaPineTree;
class WorldGenBlockBlob;
class BiomeTaiga final : public Biome {
public:
    enum class Type {
        NORMAL,
        MEGA,
        MEGA_SPRUCE
    };

    BiomeTaiga(const BiomeTaiga::Type typeIn, MU std::string biomeName, c_float baseHeight, c_float heightVariation,
               c_bool enableSnow, c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature), type(typeIn) {
        this->decorator->treesPerChunk = 10;

        if (type != BiomeTaiga::Type::MEGA && type != BiomeTaiga::Type::MEGA_SPRUCE) {
            this->decorator->mushroomsPerChunk = 1;
        } else {
            this->decorator->grassPerChunk = 7;
            this->decorator->deadBushPerChunk = 1;
            this->decorator->mushroomsPerChunk = 3;
        }
    }

    void genTerrainBlocks(i64 worldSeed, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x, c_int z,
                          c_double noiseVal) override;

    void decorate(ChunkPrimer *worldIn, RNG &rng, const Pos2D &pos) override;

    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

    const WorldGenerator *getRandomWorldGenForGrass(RNG &rng) const override;

private:
    static const WorldGenTaiga1 PINE_GENERATOR;
    static const WorldGenTaiga2 SPRUCE_GENERATOR;
    static const WorldGenMegaPineTree MEGA_PINE_GENERATOR;
    static const WorldGenMegaPineTree MEGA_SPRUCE_GENERATOR;
    static const WorldGenBlockBlob FOREST_ROCK_GENERATOR;
    BiomeTaiga::Type type;
};

class BiomeSwamp final : public Biome {
public:
    BiomeSwamp(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
               c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {
        this->decorator->treesPerChunk = 2;
        this->decorator->flowersPerChunk = 1;
        this->decorator->deadBushPerChunk = 1;
        this->decorator->mushroomsPerChunk = 8;
        this->decorator->reedsPerChunk = 10;
        this->decorator->clayPerChunk = 1;
        this->decorator->waterlilyPerChunk = 4;
        this->decorator->sandPatchesPerChunk = 0;
        this->decorator->gravelPatchesPerChunk = 0;
        this->decorator->grassPerChunk = 5;
    }

    void genTerrainBlocks(i64 worldSeed, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x, c_int z,
                          c_double noiseVal) override;

    void decorate(ChunkPrimer *worldIn, RNG &rng, const Pos2D &pos) override;

    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

    BlockFlower::EnumFlowerType pickRandomFlower(RNG &rng, const Pos2D &pos) const override;

private:
    static const WorldGenSwamp SWAMP_FEATURE;
};

class BiomeRiver final : public Biome {
public:
    BiomeRiver(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
               c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {
    }
};

class BiomeSnow final : public Biome {
public:
    MU bool superIcy;

    BiomeSnow(c_bool superIcy, MU std::string biomeName, c_float baseHeight, c_float heightVariation,
              c_bool enableSnow, c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {
        this->superIcy = superIcy;
        if (superIcy) this->topBlock = &lce::blocks::SNOW_BLOCK;
    }

    void decorate(ChunkPrimer *worldIn, RNG &rng, const Pos2D &pos) override;

    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

private:
    WorldGenIceSpike iceSpike;
    WorldGenIcePath icePatch = WorldGenIcePath(4);
};

class BiomeMushroomIsland final : public Biome {
public:
    BiomeMushroomIsland(MU std::string biomeName, c_float baseHeight, c_float heightVariation,
                        c_bool enableSnow, c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, &lce::blocks::MYCELIUM) {
        this->decorator->treesPerChunk = -100;
        this->decorator->flowersPerChunk = -100;
        this->decorator->grassPerChunk = -100;
        this->decorator->mushroomsPerChunk = 1;
        this->decorator->bigMushroomsPerChunk = 1;
    }
};

class BiomeBeach final : public Biome {
public:
    BiomeBeach(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
               c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, &lce::blocks::SAND,
                &lce::blocks::SAND) {
        this->decorator->treesPerChunk = -999;
    }
};

class BiomeJungle final : public Biome {
public:
    BiomeJungle(const bool isEdgeIn, MU std::string biomeName, c_float baseHeight, c_float heightVariation,
                c_bool enableSnow, c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature), isEdge(isEdgeIn) {
        if (isEdgeIn) this->decorator->treesPerChunk = 2;
        else {
            this->decorator->treesPerChunk = 50;
        }

        this->decorator->grassPerChunk = 25;
        this->decorator->flowersPerChunk = 4;
    }

    void decorate(ChunkPrimer *worldIn, RNG &rng, const Pos2D &pos) override;

    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

    const WorldGenerator *getRandomWorldGenForGrass(RNG &rng) const override;

private:
    bool isEdge;
};

class BiomeStoneBeach final : public Biome {
public:
    BiomeStoneBeach(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
                    c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, &lce::blocks::STONE,
                &lce::blocks::STONE) {
        this->decorator->treesPerChunk = -999;
    }
};

class BiomeSavanna : public Biome {
public:
    BiomeSavanna(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
                 c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {
        this->decorator->treesPerChunk = 1;
        this->decorator->flowersPerChunk = 4;
        this->decorator->grassPerChunk = 20;
    }

    void decorate(ChunkPrimer *worldIn, RNG &rng, const Pos2D &pos) override;

    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

private:
    static const WorldGenSavannaTree SAVANNA_TREE;
};

class BiomeMesa final : public Biome {
public:
    i64 worldSeed = 0;
    std::vector<lce::blocks::Block const *> clayBands{};
    NoiseGeneratorPerlin pillarNoise{};
    NoiseGeneratorPerlin pillarRoofNoise{};
    NoiseGeneratorPerlin clayBandsOffsetNoise{};
    bool brycePillars = false;
    bool hasForest = false;

    BiomeMesa(c_bool hasBrycePillars, c_bool hasForest, MU std::string biomeName, c_float baseHeight,
              c_float heightVariation, c_bool enableSnow, c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature),
          brycePillars(hasBrycePillars),
          hasForest(hasForest) {
        this->topBlock = &lce::blocks::RED_SAND;
        this->fillerBlock = &lce::blocks::ORANGE_HARDENED_CLAY;
        this->decorator->treesPerChunk = -999;
        this->decorator->deadBushPerChunk = 20;
        this->decorator->reedsPerChunk = 3;
        this->decorator->cactiPerChunk = 5;
        this->decorator->flowersPerChunk = 0;

        if (hasForest) {
            this->decorator->treesPerChunk = 5;
        }
    }

    void genTerrainBlocks(i64 worldSeedIn, RNG &rng, ChunkPrimer *chunkPrimerIn, int x, int z,
                          double noiseVal) override;

    void generateClayBands(c_i64 seed);

    lce::blocks::Block const *getClayBand(c_int x, c_int y);

    BiomeDecorator *createBiomeDecorator() const override;

    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

    class Decorator : public BiomeDecorator {
    protected:
        void generateOres(ChunkPrimer *chunk, RNG &rng) override;
    };
};

class MU BiomeSavannaMutated final : public BiomeSavanna {
public:
    MU BiomeSavannaMutated(MU std::string biomeName, c_float baseHeight, c_float heightVariation,
                           c_bool enableSnow, c_float temperature)
        : BiomeSavanna(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature) {
        this->decorator->treesPerChunk = 2;
        this->decorator->flowersPerChunk = 2;
        this->decorator->grassPerChunk = 5;
    }

    void genTerrainBlocks(i64 worldSeed, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x, c_int z,
                          c_double noiseVal) override;

    void decorate(ChunkPrimer *worldIn, RNG &rng, const Pos2D &pos) override;
};
