#pragma once

#include "WorldGenAbstractTree.hpp"

class World;

/**
 * @class WorldGenSwamp
 * @brief A final class responsible for generating swamp tree structures in the world.
 * Inherits from WorldGenAbstractTree and provides methods for tree generation and vine placement.
 */
class WorldGenSwamp final : public WorldGenAbstractTree {
public:
    /**
     * @brief Generates a swamp tree at the specified position in the world.
     * @param worldIn Pointer to the World object where the tree will be generated.
     * @param rand Reference to the random number generator.
     * @param position The position where the tree should be generated.
     * @return True if the tree generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rand, const Pos3D &position) const override;

private:
    /**
     * @brief Adds a vine block at the specified position in the world with the given facing direction.
     * @param worldIn Pointer to the World object where the vine will be placed.
     * @param pos The position where the vine block should be placed.
     * @param facing The direction the vine block should face.
     */
    static void addVine(World *worldIn, const Pos3D &pos, EnumFacing facing);
};
