#pragma once
#include "WorldGenAbstractTree.hpp"
#include "lce/blocks/blocks.hpp"
#include "LegacyCubiomes/chunk_generator/ChunkPrimer.hpp"
#include "LegacyCubiomes/utils/enums.hpp"
#include "LegacyCubiomes/utils/Pos3DTemplate.hpp"
#include "LegacyCubiomes/utils/rng.hpp"

class ChunkPrimer;

class WorldGenTrees : public WorldGenAbstractTree {
    static const lce::blocks::Block *DEFAULT_TRUNK;
    static const lce::blocks::Block *DEFAULT_LEAF;
public:
    /** The minimum height of a generated tree. */
    const int minTreeHeight;

    /** True if this tree should grow Vines. */
    const bool vinesGrow;

    /** The metadata value of the wood to use in tree generation. */
    const lce::blocks::Block *metaWood;

    /** The metadata value of the leaves to use in tree generation. */
    const lce::blocks::Block *metaLeaves;

public:
    WorldGenTrees() : WorldGenTrees(4, DEFAULT_TRUNK, DEFAULT_LEAF, false) {
    }

    WorldGenTrees(const int minTreeHeight, const lce::blocks::Block *metaWood, const lce::blocks::Block *metaLeaves,
                  const bool vinesGrow) : minTreeHeight(minTreeHeight), vinesGrow(vinesGrow), metaWood(metaWood),
                                          metaLeaves(metaLeaves) {
    }

    bool generate(World * worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    static void placeCocoa(World *worldIn, const Pos3D &pos, FACING side, int age);
    static void addHangingVine(World *worldIn, const Pos3D &pos, FACING direction);
};
