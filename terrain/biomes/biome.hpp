#pragma once

#include <map>
#include <string>
#include <utility>

#include "terrain/noise/NoiseGenerator.hpp"

#include "lce/blocks/blockID.hpp"
#include "lce/blocks/blocksInit.hpp"

#include "BiomeDecorator.hpp"

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

/**
 * @class Biome
 * @brief Represents a biome in the world with specific properties and behavior.
 *
 * This class defines the characteristics of a biome, such as height, temperature, and decoration.
 * It also provides methods for biome-specific terrain generation and decoration.
 */
class Biome {
public:
    static constexpr int SNOW_CHANCE_HEIGHT = 10; ///< Height threshold for snow chance.

    /**
     * @brief Map of biomes with snowing conditions based on height.
     *
     * Biomes not in this map will never snow. The map is generated based on the output
     * of `get_min_temp_biomes.cpp` in the tests directory.
     */
    static const std::map<biome_t, int> SNOW_BIOMES;

    static const NoiseGeneratorPerlin TEMPERATURE_NOISE; ///< Noise generator for temperature.
    static const NoiseGeneratorPerlin INFO_NOISE; ///< Noise generator for biome information.
    static const WorldGenDoublePlant DOUBLE_PLANT_GENERATOR; ///< Generator for double plants.

    static const WorldGenTrees TREE_FEATURE; ///< Generator for standard trees.
    static const WorldGenBigTree BIG_TREE_FEATURE; ///< Generator for big trees.

    static std::map<biome_t, Biome *> registry; ///< Registry of all biomes by ID.

    biome_t biomeID = biome_t::ocean; ///< Unique ID of the biome.
    std::string biomeName; ///< Name of the biome.

    MU float baseHeight; ///< Base height of the biome. Default is 0.1.
    MU float heightVariation; ///< Height variation of the biome. Default is 0.2.
    float temperature; ///< Temperature of the biome. Default is 0.5.
    MU bool enableSnow; ///< Indicates if snow is enabled for the biome.
    float rainFall; ///< Rainfall in the biome. Default is 0.5.
    uint32_t waterColor; ///< Color of the water in the biome. Default is 16777215.

    lce::BlockState topBlock = lce::BlocksInit::GRASS.getState(); ///< Top block of the biome.
    lce::BlockState fillerBlock = lce::BlocksInit::DIRT.getState(); ///< Filler block of the biome.

    BiomeDecorator *decorator = this->createBiomeDecorator(); ///< Biome decorator instance.

    /**
     * @brief Retrieves a biome by its ID.
     * @param id The ID of the biome.
     * @return Pointer to the biome.
     */
    static Biome *getBiomeForId(const biome_t id) {
        return registry.at(id);
    }

    /**
     * @brief Registers a biome with a specific ID.
     * @param id The ID of the biome.
     * @param biome Pointer to the biome to register.
     */
    static void registerBiome(int id, Biome *biome) {
        biome->biomeID = static_cast<biome_t>(id);
        registry.emplace(biome->biomeID, biome);
    }

    /**
     * @brief Constructs a Biome with specified properties.
     * @param biomeNameIn Name of the biome.
     * @param baseHeightIn Base height of the biome.
     * @param heightVariationIn Height variation of the biome.
     * @param enableSnowIn Whether snow is enabled.
     * @param temperatureIn Temperature of the biome.
     * @param rainFallIn Rainfall in the biome. Default is 0.5.
     * @param waterColorIn Water color in the biome. Default is 0xA5F5AF44.
     */
    Biome(std::string biomeNameIn, c_float baseHeightIn, c_float heightVariationIn, c_bool enableSnowIn,
          c_float temperatureIn, c_float rainFallIn = 0.5f, c_u32 waterColorIn = 0xA5F5AF44)
        : biomeName(std::move(biomeNameIn)), baseHeight(baseHeightIn), heightVariation(heightVariationIn),
          temperature(temperatureIn), enableSnow(enableSnowIn), rainFall(rainFallIn), waterColor(waterColorIn) {
    }

    /**
     * @brief Constructs a Biome with a specified top block.
     * @param biomeNameIn Name of the biome.
     * @param baseHeightIn Base height of the biome.
     * @param heightVariationIn Height variation of the biome.
     * @param enableSnowIn Whether snow is enabled.
     * @param temperatureIn Temperature of the biome.
     * @param rainFallIn Rainfall in the biome.
     * @param topBlockIn Top block of the biome.
     */
    Biome(std::string biomeNameIn, c_float baseHeightIn, c_float heightVariationIn, c_bool enableSnowIn,
          c_float temperatureIn, c_float rainFallIn, lce::BlockState topBlockIn)
        : Biome(std::move(biomeNameIn), baseHeightIn, heightVariationIn, enableSnowIn, temperatureIn, rainFallIn) {
        this->topBlock = topBlockIn;
    }

    /**
     * @brief Constructs a Biome with specified top and filler blocks.
     * @param biomeNameIn Name of the biome.
     * @param baseHeightIn Base height of the biome.
     * @param heightVariationIn Height variation of the biome.
     * @param enableSnowIn Whether snow is enabled.
     * @param temperatureIn Temperature of the biome.
     * @param rainFallIn Rainfall in the biome.
     * @param topBlockIn Top block of the biome.
     * @param fillerBlockIn Filler block of the biome.
     */
    Biome(std::string biomeNameIn, c_float baseHeightIn, c_float heightVariationIn, c_bool enableSnowIn,
          c_float temperatureIn, c_float rainFallIn, lce::BlockState topBlockIn,
          lce::BlockState fillerBlockIn)
        : Biome(std::move(biomeNameIn), baseHeightIn, heightVariationIn, enableSnowIn, temperatureIn, rainFallIn,
                topBlockIn) {
        this->fillerBlock = fillerBlockIn;
    }

    /**
     * @brief Virtual destructor for the Biome class.
     */
    virtual ~Biome();

protected:
    /**
     * @brief Creates a biome decorator.
     * @return Pointer to the created BiomeDecorator.
     */
    ND virtual BiomeDecorator *createBiomeDecorator() const;

public:
    /**
     * @brief Generates a big tree based on random chance.
     * @param rng Random number generator.
     * @return Pointer to the tree generator.
     */
    ND virtual const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const;

    /**
     * @brief Retrieves a random grass generator.
     * @param rng Random number generator.
     * @return Pointer to the grass generator.
     */
    ND virtual const AbstractWorldGenerator *getRandomWorldGenForGrass(RNG &rng) const;

    /**
     * @brief Picks a random flower type.
     * @param rng Random number generator.
     * @param pos Position in the world.
     * @return The type of flower.
     */
    ND virtual BlockFlower::EnumFlowerType pickRandomFlower(RNG &rng, const Pos2D &pos) const;

    /**
     * @brief Checks if the biome is snowy.
     * @return True if the biome is snowy, false otherwise.
     */
    ND bool isSnowyBiome() const;

    /**
     * @brief Checks if the biome has an ideal temperature at a position.
     * @param pos Position in the world.
     * @return True if the temperature is ideal, false otherwise.
     */
    ND bool hasIdealTemperature(const Pos3D &pos) const;

    /**
     * @brief Retrieves the temperature as a float at a position.
     * @param pos Position in the world.
     * @return The temperature as a float.
     */
    ND float getFloatTemperature(const Pos3D &pos) const;

    /**
     * @brief Decorates the biome at a specific position.
     * @param worldIn Pointer to the world.
     * @param rng Random number generator.
     * @param pos Position in the world.
     */
    virtual void decorate(World *worldIn, RNG &rng, const Pos2D &pos);

    /**
     * @brief Retrieves the water color of the biome.
     * @return The water color as a 32-bit integer.
     */
    uint32_t getWaterColor() const;

    /**
     * @brief Retrieves the rainfall of the biome.
     * @return The rainfall as a float.
     */
    float getRainfall() const;

    /**
     * @brief Retrieves the grass color at a position.
     * @param pos Position in the world.
     * @return The grass color as a 32-bit integer.
     */
    virtual uint32_t getGrassColor(const Pos3D &pos) const;

    /**
     * @brief Retrieves the foliage color at a position.
     * @param pos Position in the world.
     * @return The foliage color as a 32-bit integer.
     */
    virtual uint32_t getFoliageColor(const Pos3D &pos) const;

    /**
     * @brief Generates terrain for the biome.
     * @param rng Random number generator.
     * @param chunkPrimerIn Pointer to the chunk primer.
     * @param x X-coordinate.
     * @param z Z-coordinate.
     * @param noiseVal Noise value for terrain generation.
     */
    void generateBiomeTerrain(RNG &rng, ChunkPrimer *chunkPrimerIn, int x, int z, double noiseVal) const;

    /**
     * @brief Generates terrain blocks for the biome.
     * @param worldSeed World seed.
     * @param rng Random number generator.
     * @param chunkPrimerIn Pointer to the chunk primer.
     * @param x X-coordinate.
     * @param z Z-coordinate.
     * @param noiseVal Noise value for terrain generation.
     */
    virtual void genTerrainBlocks(i64 worldSeed, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x, c_int z,
                                  c_double noiseVal);

    /**
     * @brief Registers all biomes.
     */
    static void registerBiomes();
};

/**
 * @class BiomeNone
 * @brief Represents a none biome with specific properties.
 */
class BiomeNone final : public Biome {
public:
    /**
     * @brief Constructs a BiomeNone instance.
     * @param biomeName Name of the biome.
     * @param baseHeight Base height of the biome.
     * @param heightVariation Height variation of the biome.
     * @param enableSnow Whether snow is enabled in the biome.
     * @param temperature Temperature of the biome.
     * @param waterColor Color of the water in the biome.
     */
    BiomeNone(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
               c_float temperature, c_u32 waterColor)
        : Biome(std::move(biomeName), 0, 0, false, 0, 0.0f, waterColor) {
    }
};


/**
 * @class BiomeOcean
 * @brief Represents an ocean biome with specific properties.
 */
class BiomeOcean final : public Biome {
public:
    /**
     * @brief Constructs a BiomeOcean instance.
     * @param biomeName Name of the biome.
     * @param baseHeight Base height of the biome.
     * @param heightVariation Height variation of the biome.
     * @param enableSnow Whether snow is enabled in the biome.
     * @param temperature Temperature of the biome.
     * @param waterColor Color of the water in the biome.
     */
    BiomeOcean(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
               c_float temperature, c_u32 waterColor)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.5f, waterColor) {
    }
};

/**
 * @class BiomePlains
 * @brief Represents a plains biome with specific properties and behavior.
 */
class BiomePlains final : public Biome {
public:
    bool hasSunflowers; ///< Indicates if the biome has sunflowers.

    /**
     * @brief Constructs a BiomePlains instance.
     * @param hasSunflowers Whether the biome has sunflowers.
     * @param biomeName Name of the biome.
     * @param baseHeight Base height of the biome.
     * @param heightVariation Height variation of the biome.
     * @param enableSnow Whether snow is enabled in the biome.
     * @param temperature Temperature of the biome.
     */
    BiomePlains(c_bool hasSunflowers, MU std::string biomeName, c_float baseHeight, c_float heightVariation,
                c_bool enableSnow, c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.4f),
          hasSunflowers(hasSunflowers) {
        this->decorator->treesPerChunk = 0;
        this->decorator->extraTreeChance = 0.05F;
        this->decorator->flowersPerChunk = 4;
        this->decorator->grassPerChunk = 10;
    }

    /**
     * @brief Decorates the biome at a specific position.
     * @param worldIn Pointer to the world.
     * @param rng Random number generator.
     * @param pos Position in the world.
     */
    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;

    /**
     * @brief Generates a big tree based on random chance.
     * @param rng Random number generator.
     * @return Pointer to the tree generator.
     */
    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

    /**
     * @brief Picks a random flower type.
     * @param rng Random number generator.
     * @param pos Position in the world.
     * @return The type of flower.
     */
    ND BlockFlower::EnumFlowerType pickRandomFlower(RNG &rng, const Pos2D &pos) const override;
};

/**
 * @class BiomeDesert
 * @brief Represents a desert biome with specific properties and behavior.
 */
class BiomeDesert final : public Biome {
public:
    /**
     * @brief Constructs a BiomeDesert instance.
     * @param biomeName Name of the biome.
     * @param baseHeight Base height of the biome.
     * @param heightVariation Height variation of the biome.
     * @param enableSnow Whether snow is enabled in the biome.
     * @param temperature Temperature of the biome.
     * @param waterColor Color of the water in the biome. Default is 0xA5F5AF44.
     */
    BiomeDesert(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
                c_float temperature, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.0F,
                lce::BlocksInit::SAND.getState(),
                lce::BlocksInit::SAND.getState()) {
        this->waterColor = waterColor;
        this->decorator->treesPerChunk = -999;
        this->decorator->deadBushPerChunk = 2;
        this->decorator->reedsPerChunk = 50;
        this->decorator->cactiPerChunk = 10;
    }

    /**
     * @brief Decorates the biome at a specific position.
     * @param worldIn Pointer to the world.
     * @param rng Random number generator.
     * @param pos Position in the world.
     */
    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;
};

/**
 * @class BiomeHills
 * @brief Represents a hilly biome with specific properties and behavior.
 */
class BiomeHills final : public Biome {
public:
    /**
     * @enum Type
     * @brief Defines the types of hills in the biome.
     */
    enum class Type {
        NORMAL, ///< Normal hills.
        EXTRA_TREES, ///< Hills with extra trees.
        MUTATED ///< Mutated hills.
    };

    /**
     * @brief Constructs a BiomeHills instance.
     * @param typeIn Type of the hills.
     * @param biomeName Name of the biome.
     * @param baseHeight Base height of the biome.
     * @param heightVariation Height variation of the biome.
     * @param enableSnow Whether snow is enabled in the biome.
     * @param temperature Temperature of the biome.
     * @param waterColor Color of the water in the biome. Default is 0xA5F5AF44.
     */
    BiomeHills(const Type typeIn, MU std::string biomeName, c_float baseHeight, c_float heightVariation,
               c_bool enableSnow, c_float temperature, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.3f, waterColor),
          type(typeIn) {
        if (type == Type::EXTRA_TREES) {
            this->decorator->treesPerChunk = 3;
        }
    }

    /**
     * @brief Destructor for BiomeHills.
     */
    ~BiomeHills() override {
        delete spruceGenerator;
        delete sliverfishGenerator;
    }

    /**
     * @brief Generates terrain blocks for the biome.
     * @param worldSeed World seed.
     * @param rng Random number generator.
     * @param chunkPrimerIn Pointer to the chunk primer.
     * @param x X-coordinate.
     * @param z Z-coordinate.
     * @param noiseVal Noise value for terrain generation.
     */
    void genTerrainBlocks(i64 worldSeed, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x, c_int z,
                          c_double noiseVal) override;

    /**
     * @brief Decorates the biome at a specific position.
     * @param worldIn Pointer to the world.
     * @param rng Random number generator.
     * @param pos Position in the world.
     */
    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;

    /**
     * @brief Generates a big tree based on random chance.
     * @param rng Random number generator.
     * @return Pointer to the tree generator.
     */
    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

private:
    WorldGenAbstractTree *spruceGenerator = new WorldGenTaiga2(); ///< Generator for spruce trees.
    WorldGenMinable *sliverfishGenerator = new WorldGenMinable(lce::BlocksInit::STONE_MONSTER_EGG.getState(), 9);
    ///< Generator for silverfish blocks.
    BiomeHills::Type type; ///< Type of the hills.
};

/**
 * @class BiomeForest
 * @brief Represents a forest biome with specific properties and behavior.
 */
class BiomeForest : public Biome {
public:
    /**
     * @enum Type
     * @brief Defines the types of forests in the biome.
     */
    enum class Type {
        NORMAL, ///< Standard forest.
        FLOWER, ///< Forest with abundant flowers.
        BIRCH, ///< Forest dominated by birch trees.
        ROOFED ///< Forest with dense, roofed trees.
    };

    /**
     * @brief Constructs a BiomeForest instance.
     * @param typeIn The type of forest.
     * @param biomeName Name of the biome.
     * @param baseHeight Base height of the biome.
     * @param heightVariation Height variation of the biome.
     * @param enableSnow Whether snow is enabled in the biome.
     * @param temperature Temperature of the biome.
     * @param waterColor Color of the water in the biome. Default is 0xA5F5AF44.
     */
    BiomeForest(const Type typeIn, MU std::string biomeName, c_float baseHeight, c_float heightVariation,
                c_bool enableSnow, c_float temperature, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.8f, waterColor),
          type(typeIn) {
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

    /**
     * @brief Decorates the biome at a specific position.
     * @param worldIn Pointer to the world.
     * @param rng Random number generator.
     * @param pos Position in the world.
     */
    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;

    /**
     * @brief Generates a big tree based on random chance.
     * @param rng Random number generator.
     * @return Pointer to the tree generator.
     */
    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

    /**
     * @brief Picks a random flower type.
     * @param rng Random number generator.
     * @param pos Position in the world.
     * @return The type of flower.
     */
    ND BlockFlower::EnumFlowerType pickRandomFlower(RNG &rng, const Pos2D &pos) const override;

    /**
     * @brief Retrieves the grass color at a position.
     * @param pos Position in the world.
     * @return The grass color as a 32-bit integer.
     */
    uint32_t getGrassColor(const Pos3D &pos) const override;

private:
    /**
     * @brief Adds mushrooms to the biome at a specific position.
     * @param worldIn Pointer to the world.
     * @param rng Random number generator.
     * @param pos Position in the world.
     */
    void addMushrooms(World *worldIn, RNG &rng, const Pos2D &pos) const;

    /**
     * @brief Adds double plants to the biome at a specific position.
     * @param worldIn Pointer to the world.
     * @param rng Random number generator.
     * @param pos Position in the world.
     * @param amount Number of double plants to add.
     */
    static void addDoublePlants(World *worldIn, RNG &rng, const Pos2D &pos, int amount);

    BiomeForest::Type type; ///< The type of forest.

protected:
    static const WorldGenBirchTree SUPER_BIRCH_TREE; ///< Generator for super birch trees.
    static const WorldGenBirchTree BIRCH_TREE; ///< Generator for birch trees.
    static const WorldGenCanopyTree ROOF_TREE; ///< Generator for roofed trees.
};

/**
 * @class BiomeForestMutated
 * @brief Represents a mutated forest biome with birch trees.
 */
class BiomeForestMutated final : public BiomeForest {
public:
    /**
     * @brief Constructs a BiomeForestMutated instance.
     * @param biomeName Name of the biome.
     * @param baseHeight Base height of the biome.
     * @param heightVariation Height variation of the biome.
     * @param enableSnow Whether snow is enabled in the biome.
     * @param temperature Temperature of the biome.
     */
    BiomeForestMutated(MU std::string biomeName, c_float baseHeight, c_float heightVariation,
                       c_bool enableSnow, c_float temperature)
        : BiomeForest(BiomeForest::Type::BIRCH, std::move(biomeName), baseHeight, heightVariation, enableSnow,
                      temperature, 0x80CA8900) {
    }

    /**
     * @brief Generates a big tree based on random chance.
     * @param rng Random number generator.
     * @return Pointer to the tree generator.
     */
    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;
};

class WorldGenTaiga1;
class WorldGenTaiga2;
class WorldGenMegaPineTree;
class WorldGenBlockBlob;

/**
 * @class BiomeTaiga
 * @brief Represents a taiga biome with specific properties and behavior.
 */
class BiomeTaiga final : public Biome {
public:
    /**
     * @enum Type
     * @brief Defines the types of taiga biomes.
     */
    enum class Type {
        NORMAL, ///< Standard taiga biome.
        MEGA, ///< Mega taiga biome with larger trees.
        MEGA_SPRUCE ///< Mega spruce taiga biome with spruce trees.
    };

    /**
     * @brief Constructs a BiomeTaiga instance.
     * @param typeIn The type of taiga biome.
     * @param biomeName Name of the biome.
     * @param baseHeight Base height of the biome.
     * @param heightVariation Height variation of the biome.
     * @param enableSnow Whether snow is enabled in the biome.
     * @param temperature Temperature of the biome.
     * @param waterColor Color of the water in the biome. Default is 0xA5F5AF44.
     */
    BiomeTaiga(const Type typeIn, MU std::string biomeName, c_float baseHeight, c_float heightVariation,
               c_bool enableSnow, c_float temperature, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.8f, waterColor),
          type(typeIn) {
        this->decorator->treesPerChunk = 10;

        if (type != Type::MEGA && type != Type::MEGA_SPRUCE) {
            this->decorator->mushroomsPerChunk = 1;
        } else {
            this->decorator->grassPerChunk = 7;
            this->decorator->deadBushPerChunk = 1;
            this->decorator->mushroomsPerChunk = 3;
        }
    }

    /**
     * @brief Generates terrain blocks for the taiga biome.
     * @param worldSeed World seed.
     * @param rng Random number generator.
     * @param chunkPrimerIn Pointer to the chunk primer.
     * @param x X-coordinate.
     * @param z Z-coordinate.
     * @param noiseVal Noise value for terrain generation.
     */
    void genTerrainBlocks(i64 worldSeed, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x, c_int z,
                          c_double noiseVal) override;

    /**
     * @brief Decorates the taiga biome at a specific position.
     * @param worldIn Pointer to the world.
     * @param rng Random number generator.
     * @param pos Position in the world.
     */
    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;

    /**
     * @brief Generates a big tree based on random chance.
     * @param rng Random number generator.
     * @return Pointer to the tree generator.
     */
    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

    /**
     * @brief Retrieves a random grass generator.
     * @param rng Random number generator.
     * @return Pointer to the grass generator.
     */
    const AbstractWorldGenerator *getRandomWorldGenForGrass(RNG &rng) const override;

    static const WorldGenTaiga2 SPRUCE_GENERATOR; ///< Generator for spruce trees.
    static const WorldGenTallGrass FERN_GENERATOR; ///< Generator for ferns.
    static const WorldGenTallGrass GRASS_GENERATOR; ///< Generator for grass.

private:
    static const WorldGenTaiga1 PINE_GENERATOR; ///< Generator for pine trees.
    static const WorldGenMegaPineTree MEGA_PINE_GENERATOR; ///< Generator for mega pine trees.
    static const WorldGenMegaPineTree MEGA_SPRUCE_GENERATOR; ///< Generator for mega spruce trees.
    static const WorldGenBlockBlob FOREST_ROCK_GENERATOR; ///< Generator for forest rocks.
    BiomeTaiga::Type type; ///< The type of taiga biome.
};

/**
 * @class BiomeSwamp
 * @brief Represents a swamp biome with specific properties and behavior.
 */
class BiomeSwamp final : public Biome {
public:
    /**
     * @brief Constructs a BiomeSwamp instance.
     * @param biomeName Name of the biome.
     * @param baseHeight Base height of the biome.
     * @param heightVariation Height variation of the biome.
     * @param enableSnow Whether snow is enabled in the biome.
     * @param temperature Temperature of the biome.
     * @param waterColor Color of the water in the biome. Default is 0xA5F5AF44.
     */
    BiomeSwamp(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
               c_float temperature, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.9F, waterColor) {
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

    /**
     * @brief Generates terrain blocks for the swamp biome.
     * @param worldSeed World seed.
     * @param rng Random number generator.
     * @param chunkPrimerIn Pointer to the chunk primer.
     * @param x X-coordinate.
     * @param z Z-coordinate.
     * @param noiseVal Noise value for terrain generation.
     */
    void genTerrainBlocks(i64 worldSeed, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x, c_int z,
                          c_double noiseVal) override;

    /**
     * @brief Decorates the swamp biome at a specific position.
     * @param worldIn Pointer to the world.
     * @param rng Random number generator.
     * @param pos Position in the world.
     */
    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;

    /**
     * @brief Generates a big tree based on random chance.
     * @param rng Random number generator.
     * @return Pointer to the tree generator.
     */
    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

    /**
     * @brief Picks a random flower type.
     * @param rng Random number generator.
     * @param pos Position in the world.
     * @return The type of flower.
     */
    BlockFlower::EnumFlowerType pickRandomFlower(RNG &rng, const Pos2D &pos) const override;

    /**
     * @brief Retrieves the grass color at a position.
     * @param pos Position in the world.
     * @return The grass color as a 32-bit integer.
     */
    uint32_t getGrassColor(const Pos3D &pos) const override;

    /**
     * @brief Retrieves the foliage color at a position.
     * @param pos Position in the world.
     * @return The foliage color as a 32-bit integer.
     */
    uint32_t getFoliageColor(const Pos3D &pos) const override;

private:
    static const WorldGenSwamp SWAMP_FEATURE; ///< Generator for swamp-specific features.
};

/**
 * @class BiomeRiver
 * @brief Represents a river biome with specific properties.
 */
class BiomeRiver final : public Biome {
public:
    /**
     * @brief Constructs a BiomeRiver instance.
     * @param biomeName Name of the biome.
     * @param baseHeight Base height of the biome.
     * @param heightVariation Height variation of the biome.
     * @param enableSnow Whether snow is enabled in the biome.
     * @param temperature Temperature of the biome.
     * @param waterColor Color of the water in the biome. Default is 0xA5F5AF44.
     */
    BiomeRiver(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
               c_float temperature, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.5f, waterColor) {
    }
};

/**
 * @class BiomeSnow
 * @brief Represents a snowy biome with specific properties and behavior.
 */
class BiomeSnow final : public Biome {
public:
    MU bool superIcy; ///< Indicates if the biome is super icy.

    /**
     * @brief Constructs a BiomeSnow instance.
     * @param superIcy Whether the biome is super icy.
     * @param biomeName Name of the biome.
     * @param baseHeight Base height of the biome.
     * @param heightVariation Height variation of the biome.
     * @param enableSnow Whether snow is enabled in the biome.
     * @param temperature Temperature of the biome.
     * @param waterColor Color of the water in the biome. Default is 0xA5F5AF44.
     */
    BiomeSnow(c_bool superIcy, MU std::string biomeName, c_float baseHeight, c_float heightVariation,
              c_bool enableSnow, c_float temperature, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.5f, waterColor) {
        this->superIcy = superIcy;
        if (superIcy) this->topBlock = lce::BlocksInit::SNOW_BLOCK.getState();
    }

    /**
     * @brief Decorates the snowy biome at a specific position.
     * @param worldIn Pointer to the world.
     * @param rng Random number generator.
     * @param pos Position in the world.
     */
    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;

    /**
     * @brief Generates a big tree based on random chance.
     * @param rng Random number generator.
     * @return Pointer to the tree generator.
     */
    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

private:
    WorldGenIceSpike iceSpike; ///< Generator for ice spikes.
    WorldGenIcePath icePatch = WorldGenIcePath(4); ///< Generator for ice paths.
};

/**
 * @class BiomeMushroomIsland
 * @brief Represents a mushroom island biome with specific properties and behavior.
 */
class BiomeMushroomIsland final : public Biome {
public:
    /**
     * @brief Constructs a BiomeMushroomIsland instance.
     * @param biomeName Name of the biome.
     * @param baseHeight Base height of the biome.
     * @param heightVariation Height variation of the biome.
     * @param enableSnow Whether snow is enabled in the biome.
     * @param temperature Temperature of the biome.
     * @param waterColor Color of the water in the biome. Default is 0xA5F5AF44.
     */
    BiomeMushroomIsland(MU std::string biomeName, c_float baseHeight, c_float heightVariation,
                        c_bool enableSnow, c_float temperature, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 1.0F,
                lce::BlocksInit::MYCELIUM.getState()) {
        this->waterColor = waterColor;
        this->decorator->treesPerChunk = -100;
        this->decorator->flowersPerChunk = -100;
        this->decorator->grassPerChunk = -100;
        this->decorator->mushroomsPerChunk = 1;
        this->decorator->bigMushroomsPerChunk = 1;
    }
};

/**
 * @class BiomeBeach
 * @brief Represents a beach biome with specific properties.
 */
class BiomeBeach final : public Biome {
public:
    /**
     * @brief Constructs a BiomeBeach instance.
     * @param biomeName Name of the biome.
     * @param baseHeight Base height of the biome.
     * @param heightVariation Height variation of the biome.
     * @param enableSnow Whether snow is enabled in the biome.
     * @param temperature Temperature of the biome.
     * @param rainfall Rainfall in the biome.
     * @param waterColor Color of the water in the biome. Default is 0xA5F5AF44.
     */
    BiomeBeach(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
               c_float temperature, c_float rainfall, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, rainfall,
                lce::BlocksInit::SAND.getState(),
                lce::BlocksInit::SAND.getState()) {
        this->waterColor = waterColor;
        this->decorator->treesPerChunk = -999; ///< No trees are generated in this biome.
    }
};

/**
 * @class BiomeJungle
 * @brief Represents a jungle biome with specific properties and behavior.
 */
class BiomeJungle final : public Biome {
public:
    /**
     * @brief Constructs a BiomeJungle instance.
     * @param isEdgeIn Indicates if the biome is a jungle edge.
     * @param biomeName Name of the biome.
     * @param baseHeight Base height of the biome.
     * @param heightVariation Height variation of the biome.
     * @param enableSnow Whether snow is enabled in the biome.
     * @param temperature Temperature of the biome.
     * @param rainfall Rainfall in the biome.
     * @param waterColor Color of the water in the biome. Default is 0xA5F5AF44.
     */
    BiomeJungle(const bool isEdgeIn, MU std::string biomeName, c_float baseHeight, c_float heightVariation,
                c_bool enableSnow, c_float temperature, c_float rainfall, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, rainfall, waterColor),
          isEdge(isEdgeIn) {
        if (isEdgeIn) {
            this->decorator->treesPerChunk = 2; ///< Fewer trees for jungle edges.
        } else {
            this->decorator->treesPerChunk = 50; ///< Dense trees for standard jungle.
        }

        this->decorator->grassPerChunk = 25; ///< Abundant grass in the jungle.
        this->decorator->flowersPerChunk = 4; ///< Moderate number of flowers in the jungle.
    }

    /**
     * @brief Decorates the jungle biome at a specific position.
     * @param worldIn Pointer to the world.
     * @param rng Random number generator.
     * @param pos Position in the world.
     */
    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;

    /**
     * @brief Generates a big tree based on random chance.
     * @param rng Random number generator.
     * @return Pointer to the tree generator.
     */
    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

    /**
     * @brief Retrieves a random grass generator.
     * @param rng Random number generator.
     * @return Pointer to the grass generator.
     */
    const AbstractWorldGenerator *getRandomWorldGenForGrass(RNG &rng) const override;

    static constexpr int JUNGLE_TREE_HEIGHT_VARIATION = 7; ///< Variation in jungle tree heights.

private:
    bool isEdge; ///< Indicates if the biome is a jungle edge.
    static const WorldGenMegaJungle MEGA_JUNGLE_FEATURE; ///< Generator for mega jungle trees.
    static const WorldGenTrees JUNGLE_TREES[JUNGLE_TREE_HEIGHT_VARIATION]; ///< Array of jungle tree generators.
};

/**
 * @class BiomeStoneBeach
 * @brief Represents a stone beach biome with specific properties.
 */
class BiomeStoneBeach final : public Biome {
public:
    /**
     * @brief Constructs a BiomeStoneBeach instance.
     * @param biomeName Name of the biome.
     * @param baseHeight Base height of the biome.
     * @param heightVariation Height variation of the biome.
     * @param enableSnow Whether snow is enabled in the biome.
     * @param temperature Temperature of the biome.
     */
    BiomeStoneBeach(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
                    c_float temperature)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.3f,
                lce::BlocksInit::STONE.getState(),
                lce::BlocksInit::STONE.getState()) {
        this->waterColor = 0xA5BB670D; ///< Sets the water color specific to the stone beach biome.
        this->decorator->treesPerChunk = -999; ///< No trees are generated in this biome.
    }
};

/**
 * @class BiomeSavanna
 * @brief Represents a savanna biome with specific properties and behavior.
 */
class BiomeSavanna : public Biome {
public:
    /**
     * @brief Constructs a BiomeSavanna instance.
     * @param biomeName Name of the biome.
     * @param baseHeight Base height of the biome.
     * @param heightVariation Height variation of the biome.
     * @param enableSnow Whether snow is enabled in the biome.
     * @param temperature Temperature of the biome.
     * @param waterColor Color of the water in the biome. Default is 0xA5F5AF44.
     */
    BiomeSavanna(MU std::string biomeName, c_float baseHeight, c_float heightVariation, c_bool enableSnow,
                 c_float temperature, c_u32 waterColor = 0xA5F5AF44)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.0F, waterColor) {
        this->decorator->treesPerChunk = 1; ///< Number of trees generated per chunk.
        this->decorator->flowersPerChunk = 4; ///< Number of flowers generated per chunk.
        this->decorator->grassPerChunk = 20; ///< Number of grass patches generated per chunk.
    }

    /**
     * @brief Decorates the savanna biome at a specific position.
     * @param worldIn Pointer to the world.
     * @param rng Random number generator.
     * @param pos Position in the world.
     */
    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;

    /**
     * @brief Generates a big tree based on random chance.
     * @param rng Random number generator.
     * @return Pointer to the tree generator.
     */
    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

private:
    static const WorldGenSavannaTree SAVANNA_TREE; ///< Generator for savanna trees.
};

/**
 * @class BiomeMesa
 * @brief Represents a mesa biome with unique terrain features and properties.
 */
class BiomeMesa final : public Biome {
public:
    i64 worldSeed = 0; ///< The world seed used for terrain generation.
    std::vector<lce::BlockState> clayBands{}; ///< A collection of clay bands for the mesa biome.
    NoiseGeneratorPerlin pillarNoise{}; ///< Noise generator for mesa pillars.
    NoiseGeneratorPerlin pillarRoofNoise{}; ///< Noise generator for mesa pillar roofs.
    NoiseGeneratorPerlin clayBandsOffsetNoise{}; ///< Noise generator for clay band offsets.
    bool brycePillars = false; ///< Indicates if the biome has Bryce-like pillars.
    bool hasForest = false; ///< Indicates if the biome contains forested areas.

    /**
     * @brief Constructs a BiomeMesa instance.
     * @param hasBrycePillars Whether the biome has Bryce-like pillars.
     * @param hasForest Whether the biome contains forested areas.
     * @param biomeName Name of the biome.
     * @param baseHeight Base height of the biome.
     * @param heightVariation Height variation of the biome.
     * @param enableSnow Whether snow is enabled in the biome.
     * @param temperature Temperature of the biome.
     * @param waterColor Color of the water in the biome. Default is 0x80CA8900.
     */
    BiomeMesa(c_bool hasBrycePillars, c_bool hasForest, MU std::string biomeName, c_float baseHeight,
              c_float heightVariation, c_bool enableSnow, c_float temperature, c_u32 waterColor = 0x80CA8900)
        : Biome(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, 0.0F, waterColor),
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

    /**
     * @brief Generates terrain blocks for the mesa biome.
     * @param worldSeedIn The world seed.
     * @param rng Random number generator.
     * @param chunkPrimerIn Pointer to the chunk primer.
     * @param x X-coordinate.
     * @param z Z-coordinate.
     * @param noiseVal Noise value for terrain generation.
     */
    void genTerrainBlocks(i64 worldSeedIn, RNG &rng, ChunkPrimer *chunkPrimerIn, int x, int z,
                          double noiseVal) override;

    /**
     * @brief Generates the clay bands for the mesa biome.
     * @param seed The seed used for generating clay bands.
     */
    void generateClayBands(c_i64 seed);

    /**
     * @brief Retrieves the clay band at a specific position.
     * @param x X-coordinate.
     * @param y Y-coordinate.
     * @return The clay band block state.
     */
    lce::BlockState getClayBand(c_int x, c_int y);

    /**
     * @brief Creates a biome decorator for the mesa biome.
     * @return Pointer to the created BiomeDecorator.
     */
    ND BiomeDecorator *createBiomeDecorator() const override;

    /**
     * @brief Generates a big tree based on random chance.
     * @param rng Random number generator.
     * @return Pointer to the tree generator.
     */
    const WorldGenAbstractTree *genBigTreeChance(RNG &rng) const override;

    /**
     * @brief Retrieves the foliage color at a specific position.
     * @param pos Position in the world.
     * @return The foliage color as a 32-bit integer.
     */
    uint32_t getFoliageColor(const Pos3D &pos) const override;

    /**
     * @brief Retrieves the grass color at a specific position.
     * @param pos Position in the world.
     * @return The grass color as a 32-bit integer.
     */
    uint32_t getGrassColor(const Pos3D &pos) const override;

    /**
     * @class Decorator
     * @brief Custom decorator for the mesa biome.
     */
    class Decorator final : public BiomeDecorator {
    protected:
        /**
         * @brief Generates ores specific to the mesa biome.
         * @param world Pointer to the world.
         * @param rng Random number generator.
         */
        void generateOres(World *world, RNG &rng) override;
    };
};

/**
 * @class BiomeSavannaMutated
 * @brief Represents a mutated savanna biome with unique properties.
 */
class MU BiomeSavannaMutated final : public BiomeSavanna {
public:
    /**
     * @brief Constructs a BiomeSavannaMutated instance.
     * @param biomeName Name of the biome.
     * @param baseHeight Base height of the biome.
     * @param heightVariation Height variation of the biome.
     * @param enableSnow Whether snow is enabled in the biome.
     * @param temperature Temperature of the biome.
     * @param waterColor Color of the water in the biome.
     */
    MU BiomeSavannaMutated(MU std::string biomeName, c_float baseHeight, c_float heightVariation,
                           c_bool enableSnow, c_float temperature, c_u32 waterColor)
        : BiomeSavanna(std::move(biomeName), baseHeight, heightVariation, enableSnow, temperature, waterColor) {
        this->decorator->treesPerChunk = 2;
        this->decorator->flowersPerChunk = 2;
        this->decorator->grassPerChunk = 5;
    }

    /**
     * @brief Generates terrain blocks for the mutated savanna biome.
     * @param worldSeed The world seed.
     * @param rng Random number generator.
     * @param chunkPrimerIn Pointer to the chunk primer.
     * @param x X-coordinate.
     * @param z Z-coordinate.
     * @param noiseVal Noise value for terrain generation.
     */
    void genTerrainBlocks(i64 worldSeed, RNG &rng, ChunkPrimer *chunkPrimerIn, c_int x, c_int z,
                          c_double noiseVal) override;

    /**
     * @brief Decorates the mutated savanna biome at a specific position.
     * @param worldIn Pointer to the world.
     * @param rng Random number generator.
     * @param pos Position in the world.
     */
    void decorate(World *worldIn, RNG &rng, const Pos2D &pos) override;
};
