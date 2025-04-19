#pragma once

#include "WorldGenAbstractTree.hpp"

class World;

/**
 * @class WorldGenSavannaTree
 * @brief A final class responsible for generating savanna tree structures in the world.
 * Inherits from WorldGenAbstractTree and provides methods for tree generation and placement of logs and leaves.
 */
class WorldGenSavannaTree final : public WorldGenAbstractTree {
public:
    /**
     * @brief Generates a savanna tree at the specified position in the world.
     * @param worldIn Pointer to the World object where the tree will be generated.
     * @param rand Reference to the random number generator.
     * @param position The position where the tree should be generated.
     * @return True if the tree generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rand, const Pos3D &position) const override;

private:
    /**
     * @brief Places a log block at the specified position in the world.
     * @param worldIn Pointer to the World object where the log will be placed.
     * @param pos The position where the log block should be placed.
     */
    static void placeLogAt(World *worldIn, const Pos3D &pos);

    /**
     * @brief Places a leaf block at the specified position in the world.
     * @param worldIn Pointer to the World object where the leaf will be placed.
     * @param pos The position where the leaf block should be placed.
     */
    static void placeLeafAt(World *worldIn, const Pos3D &pos);
};
