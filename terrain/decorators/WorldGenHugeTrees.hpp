#pragma once

#include "WorldGenAbstractTree.hpp"

class World;

/**
 * @class WorldGenHugeTrees
 * @brief Represents a generator for huge tree structures in the world.
 * Inherits from WorldGenAbstractTree and provides methods for tree generation and growth.
 */
class WorldGenHugeTrees : public WorldGenAbstractTree {
public:
    /**
     * @brief Constructs a WorldGenHugeTrees object with specified parameters.
     * @param baseHeightIn The base height of the tree.
     * @param extraRandomHeightIn The additional random height added to the base height.
     * @param woodMetadataIn The metadata for the wood block state.
     * @param leavesMetadataIn The metadata for the leaves block state.
     */
    WorldGenHugeTrees(const int baseHeightIn, const int extraRandomHeightIn, const lce::BlockState woodMetadataIn,
                      const lce::BlockState leavesMetadataIn)
        : m_baseHeight(baseHeightIn), m_extraRandomHeight(extraRandomHeightIn), m_woodMetadata(woodMetadataIn),
          m_leavesMetadata(leavesMetadataIn) {
    }

    ~WorldGenHugeTrees() override;

protected:
    /**
     * @brief Calculates the height of the tree based on the random number generator.
     * @param rand Reference to the random number generator.
     * @return The calculated height of the tree.
     */
    int getHeight(RNG &rand) const;

    /**
     * @brief Checks if there is enough space at the specified position for the tree's leaves.
     * @param worldIn Pointer to the World object.
     * @param leavesPos The position to check for space.
     * @param height The height of the tree.
     * @return True if there is enough space, false otherwise.
     */
    static bool isSpaceAt(World *worldIn, const Pos3D &leavesPos, int height);

    /**
     * @brief Ensures that dirt blocks are present underneath the specified position.
     * @param pos The position to check.
     * @param worldIn Pointer to the World object.
     * @return True if dirt blocks are present, false otherwise.
     */
    static bool ensureDirtUnderneath(const Pos3D &pos, World *worldIn);

    /**
     * @brief Ensures that the tree can grow at the specified position.
     * @param worldIn Pointer to the World object.
     * @param rng Reference to the random number generator.
     * @param treePos The position of the tree.
     * @param height The height of the tree.
     * @return True if the tree can grow, false otherwise.
     */
    static bool ensureGrowable(World *worldIn, RNG &rng, const Pos3D &treePos, int height);

    /**
     * @brief Grows a strict layer of leaves at the specified position.
     * @param worldIn Pointer to the World object.
     * @param layerCenter The center position of the leaves layer.
     * @param width The width of the leaves layer.
     */
    void growLeavesLayerStrict(World *worldIn, const Pos3D &layerCenter, int width) const;

    /**
     * @brief Grows a layer of leaves at the specified position.
     * @param worldIn Pointer to the World object.
     * @param layerCenter The center position of the leaves layer.
     * @param width The width of the leaves layer.
     */
    void growLeavesLayer(World *worldIn, const Pos3D &layerCenter, int width) const;

    int m_baseHeight; /**< The base height of the tree. */
    int m_extraRandomHeight; /**< The additional random height added to the base height. */
    lce::BlockState m_woodMetadata; /**< The metadata for the wood block state. */
    lce::BlockState m_leavesMetadata; /**< The metadata for the leaves block state. */
};
