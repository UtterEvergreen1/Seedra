#pragma once

#include "WorldGenAbstractTree.hpp"

class World;

/**
 * @class WorldGenBirchTree
 * @brief A final class responsible for generating birch trees in the world.
 * Inherits from WorldGenAbstractTree and provides methods for birch tree generation.
 */
class WorldGenBirchTree final : public WorldGenAbstractTree {
public:
    /**
     * @brief Constructs a WorldGenBirchTree object.
     * @param use_extra_random_height A boolean indicating whether to use extra random height for the birch tree.
     */
    explicit WorldGenBirchTree(const bool use_extra_random_height)
        : useExtraRandomHeight(use_extra_random_height) {
    }

    /**
     * @brief Generates a birch tree at the specified position in the world.
     * @param worldIn Pointer to the World object where the tree will be generated.
     * @param rng Reference to the random number generator.
     * @param pos The base position of the tree.
     * @return True if the tree generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    bool useExtraRandomHeight; /**< Indicates whether to use extra random height for the birch tree. */
};
