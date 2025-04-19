#pragma once

#include "WorldGenAbstractTree.hpp"

class World;

/**
 * @class WorldGenTrees
 * @brief A final class responsible for generating tree structures in the world.
 * Inherits from WorldGenAbstractTree and provides methods for tree generation and additional features like vines and cocoa placement.
 */
class WorldGenTrees final : public WorldGenAbstractTree {
    /**
     * @brief Default block state for the tree trunk.
     */
    static const lce::BlockState DEFAULT_TRUNK;

    /**
     * @brief Default block state for the tree leaves.
     */
    static const lce::BlockState DEFAULT_LEAF;

public:
    /**
     * @brief The minimum height of a generated tree.
     */
    const int minTreeHeight;

    /**
     * @brief Indicates whether this tree should grow vines.
     */
    const bool vinesGrow;

    /**
     * @brief The block state representing the type of wood used in tree generation.
     */
    const lce::BlockState metaWood;

    /**
     * @brief The block state representing the type of leaves used in tree generation.
     */
    const lce::BlockState metaLeaves;

    /**
     * @brief Default constructor that initializes the tree with default trunk and leaf states, a minimum height of 4, and no vines.
     */
    WorldGenTrees() : WorldGenTrees(4, DEFAULT_TRUNK, DEFAULT_LEAF, false) {
    }

    /**
     * @brief Constructs a WorldGenTrees object with the specified parameters.
     * @param minTreeHeight The minimum height of the tree.
     * @param metaWood The block state representing the type of wood.
     * @param metaLeaves The block state representing the type of leaves.
     * @param vinesGrow Indicates whether the tree should grow vines.
     */
    WorldGenTrees(const int minTreeHeight, const lce::BlockState metaWood, const lce::BlockState metaLeaves,
                  const bool vinesGrow)
        : minTreeHeight(minTreeHeight), vinesGrow(vinesGrow), metaWood(metaWood), metaLeaves(metaLeaves) {
    }

    /**
     * @brief Generates a tree at the specified position in the world.
     * @param worldIn Pointer to the World object where the tree will be generated.
     * @param rng Reference to the random number generator.
     * @param pos The position where the tree should be generated.
     * @return True if the tree generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    /**
     * @brief Places a cocoa block at the specified position in the world with the given facing direction and age.
     * @param worldIn Pointer to the World object where the cocoa block will be placed.
     * @param pos The position where the cocoa block should be placed.
     * @param side The direction the cocoa block should face.
     * @param age The age of the cocoa block.
     */
    static void placeCocoa(World *worldIn, const Pos3D &pos, EnumFacing side, int age);

    /**
     * @brief Adds a hanging vine block at the specified position in the world with the given facing direction.
     * @param worldIn Pointer to the World object where the vine will be placed.
     * @param pos The position where the vine block should be placed.
     * @param facing The direction the vine block should face.
     */
    static void addHangingVine(World *worldIn, const Pos3D &pos, EnumFacing facing);
};
