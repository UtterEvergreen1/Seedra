#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

/**
 * @class WorldGenBlockBlob
 * @brief A final class responsible for generating block blobs in the world.
 * Inherits from AbstractWorldGenerator and provides methods for block blob generation.
 */
class WorldGenBlockBlob final : public AbstractWorldGenerator {
public:
    /**
     * @brief Constructs a WorldGenBlockBlob object.
     * @param blockIn The block state to use for the blob.
     * @param startRadiusIn The initial radius of the blob.
     */
    WorldGenBlockBlob(const lce::BlockState blockIn, const int startRadiusIn)
        : block(blockIn), startRadius(startRadiusIn) {
    }

    /**
     * @brief Generates a block blob at the specified position in the world.
     * @param worldIn Pointer to the World object where the blob will be generated.
     * @param rng Reference to the random number generator.
     * @param pos The position where the blob should be generated.
     * @return True if the blob generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rng, const Pos3D &pos) const override;

private:
    const lce::BlockState block; /**< The block state used for the blob. */
    int startRadius; /**< The initial radius of the blob. */
};
