#pragma once

#include <unordered_map>
#include <atomic>
#include <mutex>

#include "Chunk.hpp"
#include "structures/gen/village/village.hpp"
#include "structures/gen/mineshaft/mineshaft.hpp"
#include "structures/gen/stronghold/stronghold.hpp"

#include "common/Pos2DTemplate.hpp"
#include "common/Pos3DTemplate.hpp"


class Biome;
class Generator;
class ChunkPrimer;

class World {
public:
    explicit World(Generator *g);

    ~World();

    void deleteWorld();

    Generator *getGenerator() const {
        return g;
    }

    void addChunk(const Pos2D &pos, ChunkPrimer *chunk);

    bool chunkExists(const Pos2D &pos) const;

    ChunkPrimer *getChunk(const Pos2D &pos);

    ChunkPrimer *getOrCreateChunk(const Pos2D & chunkPos);

    void createChunks(const Pos2D &pos, int radius);

    void decorateCaves(const Pos2D & theStartPosition, int radius, bool hasWaterCaves);

    void decorateChunks(const Pos2D &pos, int radius);

    void generateWorldBiomes();

    int* getWorldBiomes() const { return biomes; }

    int *getChunkBiomes(const Pos2D &pos) const;

    Biome* getBiomeAt(int x, int z) const;
    int getBiomeIdAt(int x, int z) const;

    auto& getChunks() const { return chunks; }

    void lockChunks() { chunkMutex.lock(); }

    void unlockChunks() { chunkMutex.unlock(); }

    int getBlockId(int x, int y, int z);

    int getBlockId(const Pos3D &pos);

    lce::BlockState getBlock(int x, int y, int z);

    lce::BlockState getBlock(const Pos3D &pos);

    void notifyNeighbors(int x, int y, int z);

    void setBlock(int x, int y, int z, int blockId);

    void setBlockId(const Pos3D &pos, int blockId);

    void setBlock(int x, int y, int z, int blockId, int meta);

    void setBlock(const Pos3D &pos, int blockId, int meta);

    void setBlock(int x, int y, int z, lce::BlockState blockstate);

    void setBlock(const Pos3D &pos, lce::BlockState blockstate);


    bool isAirBlock(int x, int y, int z);

    bool isAirBlock(const Pos3D &pos);

    int getHeight(int x, int z);

    Pos3D getHeight(const Pos3D &pos);

    int getTopSolidOrLiquidBlock(int x, int z);

    Pos3D getTopSolidOrLiquidBlock(const Pos3D &pos);

    ND bool isSnowyAt(int x, int z) const;

    ND bool hasIdealTemperature(const Pos3D &pos) const;

    int getPrecipitationHeight(c_int x, c_int z);

    bool canBlockFreeze(const Pos3D &pos, bool noWaterAdj);

    ND bool canBlockFreezeWater(const Pos3D &pos);

    bool canSnowAt(const Pos3D &pos, bool checkLight);

    static int getSeaLevel() { return 63; }

    void generateMineshafts();
    void generateVillages();
    void generateStrongholds();

    std::vector<gen::Village> villages;
    std::vector<gen::Mineshaft> mineshafts;
    std::vector<gen::Stronghold> strongholds;
    std::unordered_map<Pos2D, ChunkPrimer *, Pos2D::Hasher> chunks;
    BoundingBox worldBounds;
    std::mutex chunkMutex;

private:

    int* biomes = nullptr;
    std::atomic<ChunkPrimer *> lastChunk = nullptr;
    std::atomic<Pos2D> lastChunkCoords = Pos2D(-100000, -100000);


    Generator *g;

};

#include "World.inl"