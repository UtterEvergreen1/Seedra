#pragma once

#include "WorldGenHugeTrees.hpp"

class World;

/**
 * @class WorldGenMegaPineTree
 * @brief A final class responsible for generating mega pine tree structures in the world.
 * Inherits from WorldGenHugeTrees and provides methods for tree generation, sapling generation, and additional features like crowns and podzol placement.
 */
class WorldGenMegaPineTree final : public WorldGenHugeTrees {
    bool m_useBaseHeight; /**< Flag indicating whether to use the base height for tree generation. */

public:
    /**
     * @brief Constructs a WorldGenMegaPineTree object with the specified base height usage flag.
     * @param useBaseHeight A boolean indicating whether to use the base height for tree generation.
     */
    explicit WorldGenMegaPineTree(bool useBaseHeight);

    /**
     * @brief Generates a mega pine tree at the specified position in the world.
     * @param worldIn Pointer to the World object where the tree will be generated.
     * @param rand Reference to the random number generator.
     * @param position The position where the tree should be generated.
     * @return True if the tree generation was successful, false otherwise.
     */
    bool generate(World *worldIn, RNG &rand, const Pos3D &position) const override;

    /**
     * @brief Generates saplings at the specified position in the world.
     * @param worldIn Pointer to the World object where the saplings will be generated.
     * @param random Reference to the random number generator.
     * @param pos The position where the saplings should be generated.
     */
    void generateSaplings(World *worldIn, RNG &random, const Pos3D &pos) const override;

private:
    /**
     * @brief Creates the crown of the tree at the specified position.
     * @param worldIn Pointer to the World object where the crown will be created.
     * @param x The X-coordinate of the crown's position.
     * @param z The Z-coordinate of the crown's position.
     * @param y The Y-coordinate of the crown's position.
     * @param val A value influencing the crown's shape or size.
     * @param rand Reference to the random number generator.
     */
    void createCrown(World *worldIn, int x, int z, int y, int val, RNG &rand) const;

    /**
     * @brief Places a circle of podzol blocks at the specified center position.
     * @param worldIn Pointer to the World object where the podzol circle will be placed.
     * @param center The center position of the podzol circle.
     */
    static void placePodzolCircle(World *worldIn, const Pos3D &center);

    /**
     * @brief Places a podzol block at the specified position.
     * @param worldIn Pointer to the World object where the podzol block will be placed.
     * @param pos The position where the podzol block should be placed.
     */
    static void placePodzolAt(World *worldIn, const Pos3D &pos);
};
