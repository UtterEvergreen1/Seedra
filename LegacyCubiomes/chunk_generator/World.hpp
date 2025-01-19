#pragma once

#include <unordered_map>

#include "LegacyCubiomes/structures/gen/stronghold/stronghold.hpp"
#include "LegacyCubiomes/structures/gen/village/village.hpp"
#include "LegacyCubiomes/structures/gen/mineshaft/mineshaft.hpp"

#include "LegacyCubiomes/utils/Pos2DTemplate.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"

class Generator;
class ChunkPrimer;


namespace lce::blocks { class Block; }


class World {
public:
    explicit World(Generator *g) : g(g) {
    }

    ~World();

    void deleteWorld();

    Generator *getGenerator() const {
        return g;
    }

    void addChunk(const Pos2D &pos, ChunkPrimer *chunk);

    ChunkPrimer *getChunk(const Pos2D &pos) const;

    ChunkPrimer *getOrCreateChunk(const Pos2D &pos);

    void decorateChunks(const Pos2D &pos, int radius);

    int getBlockId(int x, int y, int z);

    int getBlockId(const Pos3D &pos);

    const lce::blocks::Block *getBlock(int x, int y, int z);

    const lce::blocks::Block *getBlock(const Pos3D &pos);

    void notifyNeighbors(int x, int y, int z);

    void setBlock(int x, int y, int z, int blockId);

    void setBlock(const Pos3D &pos, int blockId);

    void setBlock(int x, int y, int z, int blockId, int meta);

    void setBlock(const Pos3D &pos, int blockId, int meta);

    void setBlock(int x, int y, int z, const lce::blocks::Block *block);

    void setBlock(const Pos3D &pos, const lce::blocks::Block *block);

    void setBlock(int x, int y, int z, const lce::blocks::Block& block);

    void setBlock(const Pos3D &pos, const lce::blocks::Block& block);

    bool isAirBlock(int x, int y, int z);

    bool isAirBlock(const Pos3D &pos);

    int getHeight(int x, int z);

    Pos3D getHeight(const Pos3D &pos);

    int getTopSolidOrLiquidBlock(int x, int z);

    Pos3D getTopSolidOrLiquidBlock(const Pos3D &pos);

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
private:
    Generator *g;

};
