#pragma once

#include "WorldGenAbstractTree.hpp"

class World;

/**
 * @class WorldGenCanopyTree
 * @brief A final class responsible for generating canopy trees in the world.
 * Inherits from WorldGenAbstractTree and provides methods for canopy tree generation.
 */
class WorldGenCanopyTree final : public WorldGenAbstractTree {
public:
    /**
     * @brief Generates a canopy tree at the specified position in the world.
     * @param worldIn Pointer to the World object where the tree will be generated.
     * @param rand Reference to the random number generator.
     * @param position The base position of the tree.
     * @return True if the tree generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rand, const Pos3D &position) const override;

private:
    /**
     * @brief Places a tree of the specified height at the given position.
     * @param worldIn Pointer to the World object where the tree will be placed.
     * @param pos The base position of the tree.
     * @param height The height of the tree.
     * @return True if the tree was successfully placed, false otherwise.
     */
    static bool placeTreeOfHeight(World *worldIn, const Pos3D &pos, int height);

    /**
     * @brief Places a log block at the specified position.
     * @param worldIn Pointer to the World object where the log will be placed.
     * @param pos The position where the log block will be placed.
     */
    static void placeLogAt(World *worldIn, const Pos3D &pos);

    /**
     * @brief Places a leaf block at the specified coordinates.
     * @param worldIn Pointer to the World object where the leaf will be placed.
     * @param x The x-coordinate of the leaf block.
     * @param y The y-coordinate of the leaf block.
     * @param z The z-coordinate of the leaf block.
     */
    static void placeLeafAt(World *worldIn, int x, int y, int z);
};
