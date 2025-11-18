#pragma once

#include "AbstractWorldGenerator.hpp"

class World;

/**
 * @class WorldGenAbstractTree
 * @brief An abstract class for generating tree structures in the world.
 * Inherits from AbstractWorldGenerator and provides utility methods for tree generation.
 */
class WorldGenAbstractTree : public AbstractWorldGenerator {
public:
    ~WorldGenAbstractTree() noexcept override;

    /**
     * @brief Sets default decoration parameters for the tree generation.
     * This method can be overridden by derived classes to provide specific defaults.
     */
    virtual void setDecorationDefaults() const {}

    /**
     * @brief Generates saplings at the specified position in the world.
     * This method can be overridden by derived classes to implement specific sapling generation logic.
     * @param worldIn Pointer to the World object.
     * @param rand Reference to the random number generator.
     * @param pos The position where saplings should be generated.
     */
    virtual void generateSaplings(MU World *worldIn, MU RNG &rand, MU const Pos3D &pos) const {}

protected:
    /**
     * @brief Sets dirt at a specific location if the block at that location is not already dirt.
     * @param worldIn Pointer to the World object.
     * @param pos The position where dirt should be set.
     */
    static void setDirtAt(World *worldIn, const Pos3D &pos);

    /**
     * @brief Checks if a block can be replaced by a tree.
     * @param blockId The block ID to check.
     * @return True if the block can be replaced by a tree, false otherwise.
     */
    static bool canGrowInto(int blockId);

    /**
     * @brief Adds a vine at the specified position in the world.
     * @param worldIn Pointer to the World object.
     * @param pos The position where the vine should be added.
     * @param facing The direction the vine should face.
     */
    static void addVine(World *worldIn, const Pos3D &pos, EnumFacing facing);

    /**
     * @brief Places a tree trunk at the specified position in the world.
     * @param worldIn Pointer to the World object.
     * @param rand Reference to the random number generator.
     * @param pos The position where the trunk should be placed.
     * @param height The height of the trunk.
     * @param woodType The type of wood for the trunk.
     * @param vinesGrow Whether vines should grow on the trunk.
     * @return 0 if fallen trunk, 1 if normal trunk
     */
    static int placeTrunk(World *worldIn, RNG &rand, const Pos3D &pos, int height, lce::BlockState woodType,
                          bool vinesGrow);

    /**
     * @brief Places a fallen tree trunk at the specified position in the world.
     * @param worldIn Pointer to the World object.
     * @param rand Reference to the random number generator.
     * @param pos The position where the fallen trunk should be placed.
     * @param height The length of the fallen trunk.
     * @param woodType The type of wood for the trunk.
     */
    static void placeFallenTrunk(World *worldIn, RNG &rand, const Pos3D &pos, int height, lce::BlockState woodType);
};
