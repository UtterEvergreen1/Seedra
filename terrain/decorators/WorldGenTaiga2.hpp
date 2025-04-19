#pragma once

#include "WorldGenAbstractTree.hpp"

class World;

/**
 * @class WorldGenTaiga2
 * @brief A final class responsible for generating taiga tree structures in the world.
 * Inherits from WorldGenAbstractTree and provides methods for tree generation.
 */
class WorldGenTaiga2 final : public WorldGenAbstractTree {
public:
    /**
     * @brief Generates a taiga tree at the specified position in the world.
     * @param worldIn Pointer to the World object where the tree will be generated.
     * @param rng Reference to the random number generator.
     * @param pos The position where the tree should be generated.
     * @return True if the tree generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &pos) const override;
};
