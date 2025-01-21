#pragma once

#include "WorldGenAbstractTree.hpp"


class World;

class WorldGenTrees final : public WorldGenAbstractTree {
    static const lce::Block* DEFAULT_TRUNK;
    static const lce::Block* DEFAULT_LEAF;

public:
    /// The minimum height of a generated tree.
    const int minTreeHeight;

    /// True if this tree should grow Vines.
    const bool vinesGrow;

    /// The metadata value of the wood to use in tree generation.
    const lce::Block* metaWood;

    /// The metadata value of the leaves to use in tree generation.
    const lce::Block* metaLeaves;

    WorldGenTrees() : WorldGenTrees(4, DEFAULT_TRUNK, DEFAULT_LEAF, false) {}

    WorldGenTrees(const int minTreeHeight, const lce::Block* metaWood, const lce::Block* metaLeaves,
                  const bool vinesGrow)
        : minTreeHeight(minTreeHeight), vinesGrow(vinesGrow), metaWood(metaWood), metaLeaves(metaLeaves) {}

    bool generate(World* worldIn, RNG& rng, const Pos3D& pos) const override;

private:
    static void placeCocoa(World* worldIn, const Pos3D& pos, EnumFacing side, int age);
    static void addHangingVine(World* worldIn, const Pos3D& pos, EnumFacing facing);
};
