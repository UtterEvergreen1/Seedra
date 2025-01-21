#pragma once

#include "AbstractWorldGenerator.hpp"

class World;


class WorldGenAbstractTree : public AbstractWorldGenerator {
public:
    virtual void setDecorationDefaults() const {}

    virtual void generateSaplings(MU World* worldIn, MU RNG& rand, MU const Pos3D& pos) const {}

protected:
    /// Sets dirt at a specific location if it isn't already dirt
    static void setDirtAt(World* worldIn, const Pos3D& pos);

    /**
     *
     * @param blockId The block id to check
     * @return True if the block can be replaced by a tree
     */
    static bool canGrowInto(int blockId);

    static void addVine(World* worldIn, const Pos3D& pos, EnumFacing facing);

    static int placeTrunk(World* worldIn, RNG& rand, const Pos3D& pos, int height, const lce::Block* woodType,
                          bool vinesGrow);

    static void placeFallenTrunk(World* worldIn, RNG& rand, const Pos3D& pos, int height,
                                 const lce::Block* woodType);
};
