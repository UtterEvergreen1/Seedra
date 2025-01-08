#pragma once

#include "WorldGenerator.hpp"


namespace lce::blocks { class Block; }
class World;


class WorldGenAbstractTree : public WorldGenerator {
public:
    virtual void setDecorationDefaults() const {}

    virtual void generateSaplings(World* worldIn, RNG& rand, const Pos3D& pos) const {}

protected:
    /// Sets dirt at a specific location if it isn't already dirt
    static void setDirtAt(World* worldIn, const Pos3D& pos);

    /**
     *
     * @param blockId The block id to check
     * @return True if the block can be replaced by a tree
     */
    static bool canGrowInto(int blockId);

    static void addVine(World* worldIn, const Pos3D& pos, FACING facing);

    static int placeTrunk(World* worldIn, RNG& rand, const Pos3D& pos, int height, const lce::blocks::Block* woodType,
                          bool vinesGrow);

    static void placeFallenTrunk(World* worldIn, RNG& rand, const Pos3D& pos, int height,
                                 const lce::blocks::Block* woodType);
};
