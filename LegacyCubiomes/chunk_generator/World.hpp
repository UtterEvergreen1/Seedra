#pragma once
#include <unordered_map>
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"
#include "LegacyCubiomes/utils/Pos2DTemplate.hpp"

class World {
public:
    explicit World(Generator *g) : g(g) {
    }

    void addChunk(const Pos2D &pos, ChunkPrimer *chunk);

    ChunkPrimer *getChunk(const Pos2D &pos) const;

    ChunkPrimer *getOrCreateChunk(const Pos2D &pos);

    void decorateChunkAndNeighbors(const Pos2D &pos, int radius);

    int getBlockId(int x, int y, int z) const;

    int getBlockId(const Pos3D &pos) const;

    const lce::blocks::Block *getBlock(int x, int y, int z) const;

    const lce::blocks::Block *getBlock(const Pos3D &pos) const;

    void setBlock(int x, int y, int z, int blockId);

    void setBlock(const Pos3D &pos, int blockId);

    void setBlock(int x, int y, int z, int blockId, int meta);

    void setBlock(const Pos3D &pos, int blockId, int meta);

    void setBlock(int x, int y, int z, const lce::blocks::Block *block);

    void setBlock(const Pos3D &pos, const lce::blocks::Block *block);

    bool isAirBlock(int x, int y, int z) const;

    bool isAirBlock(const Pos3D &pos) const;

    int getHeight(int x, int z) const;

    Pos3D getHeight(const Pos3D &pos) const;

    int getTopSolidOrLiquidBlock(int x, int z) const;

    Pos3D getTopSolidOrLiquidBlock(const Pos3D &pos) const;


    int getPrecipitationHeight(c_int x, c_int z) const;

    bool canBlockFreeze(const Pos3D &pos, bool noWaterAdj) const;

    ND bool canBlockFreezeWater(const Pos3D &pos) const;

    bool canSnowAt(const Pos3D &pos, bool checkLight) const;

private:
    Generator *g;
    std::unordered_map<Pos2D, ChunkPrimer *, Pos2D::Hasher> chunks;
};
