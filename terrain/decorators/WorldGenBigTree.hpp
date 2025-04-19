#pragma once

#include "WorldGenAbstractTree.hpp"

class World;

/**
 * @class WorldGenBigTree
 * @brief A final class responsible for generating large tree structures in the world.
 * Inherits from WorldGenAbstractTree and provides methods for tree generation.
 */
class WorldGenBigTree final : public WorldGenAbstractTree {
    /**
     * @class FoliageCoordinates
     * @brief Represents the coordinates of foliage nodes and their associated branch base.
     */
    class FoliageCoordinates;

    mutable RNG rand; /**< Random number generator used for tree generation. */
    mutable World *world{}; /**< Pointer to the world where the tree will be generated. */
    mutable Pos3D basePos; /**< The base position of the tree. */
    mutable int heightLimit{}; /**< The maximum height of the tree. */
    mutable int height{}; /**< The actual height of the tree. */
    const double heightAttenuation = 0.618; /**< Factor controlling the tapering of the tree height. */
    const double branchSlope = 0.381; /**< Slope of the tree branches. */
    const double scaleWidth = 1.0; /**< Scale factor for the width of the tree. */
    const double leafDensity = 1.0; /**< Density of the tree's leaves. */
    int trunkSize = 1; /**< The size of the tree trunk. */
    int heightLimitLimit = 12; /**< The maximum allowable height limit for the tree. */
    mutable int leafDistanceLimit = 4; /**< The maximum distance leaves can be from the trunk. */
    mutable std::vector<FoliageCoordinates> foliageCoords; /**< List of foliage coordinates for the tree. */

public:
    /**
     * @brief Generates a large tree at the specified position in the world.
     * @param worldIn Pointer to the World object where the tree will be generated.
     * @param rng Reference to the random number generator.
     * @param position The base position of the tree.
     * @return True if the tree generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &position) const override;

    /**
     * @brief Sets default decoration parameters for the tree generation.
     * Overrides the base class method to set specific defaults for large trees.
     */
    void setDecorationDefaults() const override { this->leafDistanceLimit = 5; }

private:
    /**
     * @brief Generates the list of leaf nodes for the tree.
     */
    void generateLeafNodeList() const;

    /**
     * @brief Creates a cross-section of leaves at a specific position.
     * @param pos The position of the cross-section.
     * @param radius The radius of the cross-section.
     * @param block The block state to use for the leaves.
     */
    void crossSection(const Pos3D &pos, float radius, lce::BlockState block) const;

    /**
     * @brief Calculates the size of a tree layer at a specific height.
     * @param y The height of the layer.
     * @return The size of the layer.
     */
    float layerSize(int y) const;

    /**
     * @brief Calculates the size of the leaves at a specific height.
     * @param y The height of the leaves.
     * @return The size of the leaves.
     */
    float leafSize(int y) const;

    /**
     * @brief Generates a single leaf node at the specified position.
     * @param pos The position of the leaf node.
     */
    void generateLeafNode(const Pos3D &pos) const;

    /**
     * @brief Creates a limb (branch) between two positions.
     * @param start The starting position of the limb.
     * @param end The ending position of the limb.
     * @param block The block state to use for the limb.
     */
    void limb(const Pos3D &start, const Pos3D &end, lce::BlockState block) const;

    /**
     * @brief Gets the greatest distance from the origin for a given position.
     * @param pos The position to evaluate.
     * @return The greatest distance.
     */
    static int getGreatestDistance(const Pos3D &pos);

    /**
     * @brief Determines the axis of a log based on its start and end positions.
     * @param start The starting position of the log.
     * @param end The ending position of the log.
     * @return The axis of the log.
     */
    static EnumAxis getLogAxis(const Pos3D &start, const Pos3D &end);

    /**
     * @brief Generates the leaves for the tree.
     */
    void generateLeaves() const;

    /**
     * @brief Checks if a leaf node requires a base.
     * @param y The height of the leaf node.
     * @return True if the leaf node needs a base, false otherwise.
     */
    bool leafNodeNeedsBase(int y) const;

    /**
     * @brief Generates the trunk of the tree.
     */
    void generateTrunk() const;

    /**
     * @brief Generates the bases for the leaf nodes.
     */
    void generateLeafNodeBases() const;

    /**
     * @brief Checks if a line of blocks is clear between two positions.
     * @param start The starting position of the line.
     * @param end The ending position of the line.
     * @return The number of blocks that are clear.
     */
    int checkBlockLine(const Pos3D &start, const Pos3D &end) const;

    /**
     * @brief Validates if the tree can be generated at the current location.
     * @return True if the location is valid, false otherwise.
     */
    bool validTreeLocation() const;

    /**
     * @class FoliageCoordinates
     * @brief Represents the coordinates of a foliage node and its branch base.
     */
    class FoliageCoordinates : public Pos3D {
    public:
        /**
         * @brief Constructs a FoliageCoordinates object.
         * @param pos The position of the foliage node.
         * @param branch_base The base of the branch associated with the foliage node.
         */
        FoliageCoordinates(const Pos3D &pos, const int branch_base) : Pos3D(pos), branchBase(branch_base) {
        }

        /**
         * @brief Gets the branch base associated with the foliage node.
         * @return The branch base.
         */
        ND int getBranchBase() const { return this->branchBase; }

    private:
        int branchBase; /**< The base of the branch associated with the foliage node. */
    };
};
