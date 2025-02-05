#pragma once

#include <map>
#include <string>
#include <utility>

#include "terrain/carve/NoiseGenerator.hpp"

#include "lce/blocks/blockID.hpp"
#include "lce/blocks/blocksInit.hpp"

#include "BiomeDecorator.hpp"
#include "biomeID.hpp"

#include "terrain/decorators/WorldGenAbstractTree.hpp"
#include "terrain/decorators/WorldGenFlowers.hpp"
#include "terrain/decorators/WorldGenIcePath.hpp"
#include "terrain/decorators/WorldGenIceSpike.hpp"
#include "terrain/decorators/WorldGenMegaJungle.hpp"
#include "terrain/decorators/WorldGenTaiga2.hpp"
#include "terrain/decorators/WorldGenTallGrass.hpp"

#include "common/Pos3DTemplate.hpp"
#include "common/constants.hpp"
#include "common/rng.hpp"

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
    static constexpr int SNOW_CHANCE_HEIGHT = 10;
    /**
     * Biome that have a chance of snowing between y1 and y2 and always snowing above y2 and never snowing below y1
     *
     * Biomes not in this map will never snow
     *
     * Map generated based on output of get_min_temp_biomes.cpp in programs directory
     */
    static const std::map<int, int> SNOW_BIOMES;
    static const NoiseGeneratorPerlin TEMPERATURE_NOISE;
    static const NoiseGeneratorPerlin INFO_NOISE;
    static const WorldGenDoublePlant DOUBLE_PLANT_GENERATOR;

    /** The tree generator. */
    static const WorldGenTrees TREE_FEATURE;

    /** The big tree generator. */
    static const WorldGenBigTree BIG_TREE_FEATURE;

    static std::map<int, Biome *> registry;
    int biomeID;
    std::string biomeName;

    /// The base height of this biome. Default 0.1.
    MU float baseHeight;

    /// The variation from the base height of the biome. Default 0.2.
    MU float heightVariation;

    /// The temperature of the biome. Default 0.5.
    float temperature;

    /** Set to true if snow is enabled for this biome. */
    MU bool enableSnow;

    /// The rainfall in the biome. Default 0.5.
    float rainFall;

    /// The color of the water. Default 16777215.
    uint32_t waterColor;


    /// The block expected to be on the top of this biome
    lce::BlockState topBlock = lce::BlocksInit::GRASS.getState();

    /// The block to fill spots in when not on the top
    lce::BlockState fillerBlock = lce::BlocksInit::DIRT.getState();

    BiomeDecorator *decorator = this->createBiomeDecorator();

    static Biome *getBiomeForId(c_int id) { return registry.at(id); }

    static void registerBiome(int id, Biome *biome) { biome->biomeID = id; registry.emplace(id, biome);  }

    Biome(std::string biomeNameIn, c_float baseHeightIn, c_float heightVariationIn, c_bool enableSnowIn,
          c_float temperatureIn, c_float rainFallIn = 0.5f, c_u32 waterColorIn = 0xA5F5AF44)
        : biomeName(std::move(biomeNameIn)), baseHeight(baseHeightIn), heightVariation(heightVariationIn),
          temperature(temperatureIn), enableSnow(enableSnowIn), rainFall(rainFallIn), waterColor(waterColorIn) {
    }

    Biome(std::string biomeNameIn, c_float baseHeightIn, c_float heightVariationIn, c_bool enableSnowIn,
          c_float temperatureIn, c_float rainFallIn, lce::BlockState topBlockIn)
        : Biome(std::move(biomeNameIn), baseHeightIn, heightVariationIn, enableSnowIn, temperatureIn, rainFallIn) {
        this->topBlock = topBlockIn;
    }

    Biome(std::string biomeNameIn, c_float baseHeightIn, c_float heightVariationIn, c_bool enableSnowIn,
          c_float temperatureIn, c_float rainFallIn, lce::BlockState topBlockIn,
          lce::BlockState fillerBlockIn)
        : Biome(std::move(biomeNameIn), baseHeightIn, heightVariationIn, enableSnowIn, temperatureIn, rainFallIn,
                topBlockIn) {
        this->fillerBlock = fillerBlockIn;
    }

    virtual ~Biome();

protected:
    ND virtual BiomeDecorator *createBiomeDecorator() const;

public:
    ND virtual const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const;

    ND virtual const AbstractWorldGenerator *getRandomWorldGenForGrass(RNG &rng) const;

    ND virtual BlockFlower::EnumFlowerType pickRandomFlower(RNG &rng, const Pos2D &pos) const;

    ND bool isSnowyBiome() const;

    ND bool hasIdealTemperature(const Pos3D& pos) const;

    ND float getFloatTemperature(const Pos3D &pos) const;

    virtual void decorate(World *worldIn, RNG &rng, const Pos2D &pos);

    uint32_t getWaterColor() const;

    float getRainfall() const;

    virtual uint32_t getGrassColor(const Pos3D &pos) const;

    virtual uint32_t getFoliageColor(const Pos3D &pos) const;

    void generateBiomeTerrain(RNG &rng, ChunkPrimer *chunkPrimerIn, int x, int z, double noiseVal) const;

    virtual void genTerrainBlocks(i64 worldSeed, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x, c_int z,
                                  c_double noiseVal);

    static void registerBiomes();
};

class BiomeOcean final : public Biome {
public:
    BiomeOcean(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
               c_float temperature, c_u32 waterColor)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.5f, waterColor) {
    }
};

class BiomePlains final : public Biome {
public:
    bool hasSunflowers;

    BiomePlains(c_bool hasSunflowers, MU std::string biomeName, c_float baseHeight, c_float heightVariation,
                c_bool enableSnow, c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.4f),
          hasSunflowers(hasSunflowers) {
        this->decorator->treesPerChunk = 0;
        this->decorator->extraTreeChance = 0.05F;
        this->decorator->flowersPerChunk = 4;
        this->decorator->grassPerChunk = 10;
    }

    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;

    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

    ND BlockFlower::EnumFlowerType pickRandomFlower(RNG &rng, const Pos2D &pos) const override;
};

class BiomeDesert final : public Biome {
public:
    BiomeDesert(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
                c_float temperature, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.0f, lce::BlocksInit::SAND.getState(),
                lce::BlocksInit::SAND.getState()) {
        this->waterColor = waterColor;
        this->decorator->treesPerChunk = -999;
        this->decorator->deadBushPerChunk = 2;
        this->decorator->reedsPerChunk = 50;
        this->decorator->cactiPerChunk = 10;
    }

    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;
};

class BiomeHills final : public Biome {
public:
    enum class Type {
        NORMAL,
        EXTRA_TREES,
        MUTATED
    };

    BiomeHills(const Type typeIn, MU std::string biomeName, c_float baseHeight, c_float heightVariation,
               c_bool enableSnow, c_float temperature, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.3f, waterColor), type(typeIn) {
        if (type == Type::EXTRA_TREES) {
            this->decorator->treesPerChunk = 3;
        }
    }

    ~BiomeHills() override {
        delete spruceGenerator;
        delete sliverfishGenerator;
    }

    void genTerrainBlocks(i64 worldSeed, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x, c_int z,
                          c_double noiseVal) override;

    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;

    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

private:
    WorldGenAbstractTree *spruceGenerator = new WorldGenTaiga2();
    WorldGenMinable *sliverfishGenerator = new WorldGenMinable(lce::BlocksInit::STONE_MONSTER_EGG.getState(), 9);
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

    BiomeForest(const Type typeIn, MU std::string biomeName, c_float baseHeight, c_float heightVariation,
                c_bool enableSnow, c_float temperature, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.8f, waterColor), type(typeIn) {
        this->decorator->treesPerChunk = 10;
        this->decorator->grassPerChunk = 2;

        if (type == Type::FLOWER) {
            this->decorator->treesPerChunk = 6;
            this->decorator->flowersPerChunk = 100;
            this->decorator->grassPerChunk = 1;
        } else if (type == Type::ROOFED) {
            this->decorator->treesPerChunk = -999;
        }
    }

    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;

    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

    ND BlockFlower::EnumFlowerType pickRandomFlower(RNG &rng, const Pos2D &pos) const override;

    uint32_t getGrassColor(const Pos3D &pos) const override;

private:
    void addMushrooms(World *worldIn, RNG &rng, const Pos2D &pos) const;

    static void addDoublePlants(World *worldIn, RNG &rng, const Pos2D &pos, int amount);

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
                      temperature, 0x80CA8900) {
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

    BiomeTaiga(const Type typeIn, MU std::string biomeName, c_float baseHeight, c_float heightVariation,
               c_bool enableSnow, c_float temperature, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.8f, waterColor), type(typeIn) {
        this->decorator->treesPerChunk = 10;

        if (type != Type::MEGA && type != Type::MEGA_SPRUCE) {
            this->decorator->mushroomsPerChunk = 1;
        } else {
            this->decorator->grassPerChunk = 7;
            this->decorator->deadBushPerChunk = 1;
            this->decorator->mushroomsPerChunk = 3;
        }
    }

    void genTerrainBlocks(i64 worldSeed, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x, c_int z,
                          c_double noiseVal) override;

    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;

    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

    const AbstractWorldGenerator *getRandomWorldGenForGrass(RNG &rng) const override;

    static const WorldGenTaiga2 SPRUCE_GENERATOR;
    static const WorldGenTallGrass FERN_GENERATOR;
    static const WorldGenTallGrass GRASS_GENERATOR;

private:
    static const WorldGenTaiga1 PINE_GENERATOR;
    static const WorldGenMegaPineTree MEGA_PINE_GENERATOR;
    static const WorldGenMegaPineTree MEGA_SPRUCE_GENERATOR;
    static const WorldGenBlockBlob FOREST_ROCK_GENERATOR;
    BiomeTaiga::Type type;
};

class BiomeSwamp final : public Biome {
public:
    BiomeSwamp(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
               c_float temperature, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.9F, waterColor/*0xAEFFE0*//*0xE0FFAE*/) {
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

    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;

    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

    BlockFlower::EnumFlowerType pickRandomFlower(RNG &rng, const Pos2D &pos) const override;

    uint32_t getGrassColor(const Pos3D &pos) const override;

    uint32_t getFoliageColor(const Pos3D &pos) const override;

private:
    static const WorldGenSwamp SWAMP_FEATURE;
};

class BiomeRiver final : public Biome {
public:
    BiomeRiver(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
               c_float temperature, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.5f, waterColor) {
    }
};

class BiomeSnow final : public Biome {
public:
    MU bool superIcy;

    BiomeSnow(c_bool superIcy, MU std::string biomeName, c_float baseHeight, c_float heightVariation,
              c_bool enableSnow, c_float temperature, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.5f, waterColor) {
        this->superIcy = superIcy;
        if (superIcy) this->topBlock = lce::BlocksInit::SNOW_BLOCK.getState();
    }

    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;

    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

private:
    WorldGenIceSpike iceSpike;
    WorldGenIcePath icePatch = WorldGenIcePath(4);
};

class BiomeMushroomIsland final : public Biome {
public:
    BiomeMushroomIsland(MU std::string biomeName, c_float baseHeight, c_float heightVariation,
                        c_bool enableSnow, c_float temperature, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 1.0f,
                lce::BlocksInit::MYCELIUM.getState()) {
        this->waterColor = waterColor;
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
               c_float temperature, c_float rainfall, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, rainfall, lce::BlocksInit::SAND.getState(),
                lce::BlocksInit::SAND.getState()) {
        this->waterColor = waterColor;
        this->decorator->treesPerChunk = -999;
    }
};

class BiomeJungle final : public Biome {
public:
    BiomeJungle(const bool isEdgeIn, MU std::string biomeName, c_float baseHeight, c_float heightVariation,
                c_bool enableSnow, c_float temperature, c_float rainfall, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, rainfall, waterColor), isEdge(isEdgeIn) {
        if (isEdgeIn) this->decorator->treesPerChunk = 2;
        else {
            this->decorator->treesPerChunk = 50;
        }

        this->decorator->grassPerChunk = 25;
        this->decorator->flowersPerChunk = 4;
    }

    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;

    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

    const AbstractWorldGenerator *getRandomWorldGenForGrass(RNG &rng) const override;

    static constexpr int JUNGLE_TREE_HEIGHT_VARIATION = 7;

private:
    bool isEdge;
    static const WorldGenMegaJungle MEGA_JUNGLE_FEATURE;
    static const WorldGenTrees JUNGLE_TREES[JUNGLE_TREE_HEIGHT_VARIATION];
};

class BiomeStoneBeach final : public Biome {
public:
    BiomeStoneBeach(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
                    c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.3f, lce::BlocksInit::STONE.getState(),
                lce::BlocksInit::STONE.getState()) {
        this->waterColor = 0xA5BB670D;
        this->decorator->treesPerChunk = -999;
    }
};

class BiomeSavanna : public Biome {
public:
    BiomeSavanna(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
                 c_float temperature, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.0f, waterColor) {
        this->decorator->treesPerChunk = 1;
        this->decorator->flowersPerChunk = 4;
        this->decorator->grassPerChunk = 20;
    }

    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;

    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

private:
    static const WorldGenSavannaTree SAVANNA_TREE;
};

class BiomeMesa final : public Biome {
public:
    i64 worldSeed = 0;
    std::vector<lce::BlockState> clayBands{};
    NoiseGeneratorPerlin pillarNoise{};
    NoiseGeneratorPerlin pillarRoofNoise{};
    NoiseGeneratorPerlin clayBandsOffsetNoise{};
    bool brycePillars = false;
    bool hasForest = false;

    BiomeMesa(c_bool hasBrycePillars, c_bool hasForest, MU std::string biomeName, c_float baseHeight,
              c_float heightVariation, c_bool enableSnow, c_float temperature, c_u32 waterColor = 0x80CA8900)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.0f, waterColor),
          brycePillars(hasBrycePillars),
          hasForest(hasForest) {
        this->topBlock = lce::BlocksInit::RED_SAND.getState();
        this->fillerBlock = lce::BlocksInit::ORANGE_HARDENED_CLAY.getState();
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

    lce::BlockState getClayBand(c_int x, c_int y);

    ND BiomeDecorator *createBiomeDecorator() const override;

    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

    uint32_t getFoliageColor(const Pos3D &pos) const override;

    uint32_t getGrassColor(const Pos3D &pos) const override;

    class Decorator final : public BiomeDecorator {
    protected:
        void generateOres(World *world, RNG &rng) override;
    };
};

class MU BiomeSavannaMutated final : public BiomeSavanna {
public:
    MU BiomeSavannaMutated(MU std::string biomeName, c_float baseHeight, c_float heightVariation,
                           c_bool enableSnow, c_float temperature, c_u32 waterColor)
        : BiomeSavanna(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, waterColor) {
        this->decorator->treesPerChunk = 2;
        this->decorator->flowersPerChunk = 2;
        this->decorator->grassPerChunk = 5;
    }

    void genTerrainBlocks(i64 worldSeed, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x, c_int z,
                          c_double noiseVal) override;

    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;
};
