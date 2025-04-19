#pragma once

#include "WorldGenHugeTrees.hpp"

class World;

/**
 * @class WorldGenMegaJungle
 * @brief A final class responsible for generating mega jungle tree structures in the world.
 * Inherits from WorldGenHugeTrees and provides methods for tree generation and additional features like vines and crowns.
 */
class WorldGenMegaJungle final : public WorldGenHugeTrees {
public:
    /**
     * @brief Constructs a WorldGenMegaJungle object with specified parameters.
     * @param baseHeight The base height of the tree.
     * @param extraRandomHeight The additional random height added to the base height.
     * @param woodMetadata The metadata for the wood block state.
     * @param leavesMetadata The metadata for the leaves block state.
     */
    WorldGenMegaJungle(const int baseHeight, const int extraRandomHeight, const lce::BlockState woodMetadata,
                       const lce::BlockState leavesMetadata)
        : WorldGenHugeTrees(baseHeight, extraRandomHeight, woodMetadata, leavesMetadata) {
    }

    /**
     * @brief Generates a mega jungle tree at the specified position in the world.
     * @param worldIn Pointer to the World object where the tree will be generated.
     * @param rng Reference to the random number generator.
     * @param pos The position where the tree should be generated.
     * @return True if the tree generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    /**
     * @brief Places a vine at the specified position in the world.
     * @param worldIn Pointer to the World object where the vine will be placed.
     * @param rand Reference to the random number generator.
     * @param pos The position where the vine should be placed.
     * @param facing The direction the vine should face.
     */
    static void placeVine(World *worldIn, RNG &rand, const Pos3D &pos, EnumFacing facing);

    /**
     * @brief Creates the crown of the tree at the specified position.
     * @param worldIn Pointer to the World object where the crown will be created.
     * @param pos The position of the crown's center.
     * @param width The width of the crown.
     */
    void createCrown(World *worldIn, const Pos3D &pos, int width) const;
};
