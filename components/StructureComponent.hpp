#pragma once

#include "StructureVariant.hpp"
#include "BoundingBox.hpp"
#include "Piece.hpp"

#include "lce/blocks/block.hpp"
#include "common/Pos3DTemplate.hpp"
#include "common/RotationAndMirror.hpp"

/**
 * @class StructureComponent
 * @brief Represents a structural component in a world generation system.
 *
 * This class extends the Piece class and provides additional functionality for handling
 * structural components, including rotation, mirroring, and block placement within a bounding box.
 */
class World;
class ChunkPrimer;
class RNG;

class StructureComponent final : public Piece {
public:
    mutable StructureVariant variant{}; ///< The variant of the structure component.
    Rotation rotation = Rotation::NONE; ///< The rotation of the structure component.
    Mirror mirror = Mirror::NONE; ///< The mirroring mode of the structure component.

    /**
     * @brief Default constructor initializing a structure component with default values.
     */
    StructureComponent() = default;

    /**
     * @brief Constructs a structure component with a bounding box and orientation.
     *
     * @param bb The bounding box of the structure component.
     * @param facing The orientation of the structure component.
     */
    MU StructureComponent(const BoundingBox bb, const EnumFacing facing)
        : Piece(bb, facing) {
        setCoordMode(facing);
    }

    /**
     * @brief Constructs a structure component with specified bounding box coordinates and orientation.
     *
     * @param minX Minimum X coordinate of the bounding box.
     * @param minY Minimum Y coordinate of the bounding box.
     * @param minZ Minimum Z coordinate of the bounding box.
     * @param maxX Maximum X coordinate of the bounding box.
     * @param maxY Maximum Y coordinate of the bounding box.
     * @param maxZ Maximum Z coordinate of the bounding box.
     * @param facing The orientation of the structure component.
     */
    MU StructureComponent(c_int minX, c_int minY, c_int minZ, c_int maxX, c_int maxY, c_int maxZ,
                          const EnumFacing facing)
        : Piece(minX, minY, minZ, maxX, maxY, maxZ, facing) {
        setCoordMode(facing);
    }

    /**
     * @brief Constructs a structure component with specified type, depth, bounding box, orientation, and data.
     *
     * @param type The type of the piece.
     * @param depth The depth of the piece in the structure hierarchy.
     * @param bb The bounding box of the structure component.
     * @param facing The orientation of the structure component.
     * @param data Additional data associated with the structure component.
     */
    MU StructureComponent(const PieceType type, c_i8 depth, const BoundingBox &bb, const EnumFacing facing, c_int data)
        : Piece(type, depth, bb, facing, data) {
        setCoordMode(facing);
    }

    /**
     * @brief Sets the rotation and mirroring mode based on the given orientation.
     *
     * @param facing The orientation of the structure component.
     */
    void setCoordMode(const EnumFacing facing) {
        switch (facing) {
            case EnumFacing::NORTH:
            default:
                rotation = Rotation::NONE;
                mirror = Mirror::NONE;
                break;
            case EnumFacing::EAST:
                rotation = Rotation::CLOCKWISE_90;
                mirror = Mirror::NONE;
                break;
            case EnumFacing::SOUTH:
                rotation = Rotation::NONE;
                mirror = Mirror::LEFT_RIGHT;
                break;
            case EnumFacing::WEST:
                rotation = Rotation::CLOCKWISE_90;
                mirror = Mirror::LEFT_RIGHT;
                break;
        }
    }

    /**
     * @brief Copies the properties of another structure component.
     *
     * @param scIn The structure component to copy from.
     */
    MU void usingPiece(const StructureComponent &scIn) {
        minX = scIn.minX;
        maxX = scIn.maxX;
        minY = scIn.minY;
        maxY = scIn.maxY;
        minZ = scIn.minZ;
        maxZ = scIn.maxZ;
        type = scIn.type;
        facing = scIn.facing;
        rotation = scIn.rotation;
        mirror = scIn.mirror;
        data = scIn.data;
        depth = scIn.depth;
        variant = scIn.variant;
    }

    /**
     * @brief Places a block state in the world without applying any offset.
     *
     * @param worldIn The world where the block will be placed.
     * @param blockStateIn The block state to place.
     * @param x The X coordinate.
     * @param y The Y coordinate.
     * @param z The Z coordinate.
     * @param structureBB The bounding box of the structure.
     */
    MU static void setBlockStateWithoutOffset(
        World &worldIn, lce::BlockState blockStateIn, int x, int y, int z, const BoundingBox &structureBB);

    /**
     * @brief Places a block state in the world with an offset applied.
     *
     * @param worldIn The world where the block will be placed.
     * @param blockStateIn The block state to place.
     * @param x The X coordinate.
     * @param y The Y coordinate.
     * @param z The Z coordinate.
     * @param structureBB The bounding box of the structure.
     */
    MU void setBlockState(
        World &worldIn, lce::BlockState blockStateIn, int x, int y, int z,
        const BoundingBox &structureBB) const;

    /**
     * @brief Retrieves the block state at a specific position within the bounding box.
     *
     * @param worldIn The world to query.
     * @param x The X coordinate.
     * @param y The Y coordinate.
     * @param z The Z coordinate.
     * @param boundingBoxIn The bounding box of the structure.
     * @return The block state at the specified position.
     */
    MU ND lce::BlockState getBlockStateFromPos(
        World &worldIn, int x, int y, int z, const BoundingBox &boundingBoxIn) const;

    /**
     * @brief Clears blocks upwards from a specific position within the bounding box.
     *
     * @param worldIn The world to modify.
     * @param x The X coordinate.
     * @param y The Y coordinate.
     * @param z The Z coordinate.
     * @param structureBB The bounding box of the structure.
     */
    void clearCurrentPositionBlocksUpwards(
        World &worldIn, int x, int y, int z, const BoundingBox &structureBB) const;

    /**
     * @brief Replaces air and liquid blocks downwards from a specific position.
     *
     * @param worldIn The world to modify.
     * @param blockStateIn The block state to place.
     * @param x The X coordinate.
     * @param y The Y coordinate.
     * @param z The Z coordinate.
     * @param boundingBoxIn The bounding box of the structure.
     */
    void replaceAirAndLiquidDownwards(
        World &worldIn, lce::BlockState blockStateIn, int x, int y, int z,
        const BoundingBox &boundingBoxIn) const;

    /**
                                             * @brief Fills a specified area within a bounding box with blocks, using different block states for the boundary and inside.
                                             *
                                             * @param worldIn The world where the blocks will be placed.
                                             * @param bbIn The bounding box defining the area to fill.
                                             * @param minX Minimum X coordinate of the area.
                                             * @param minY Minimum Y coordinate of the area.
                                             * @param minZ Minimum Z coordinate of the area.
                                             * @param maxX Maximum X coordinate of the area.
                                             * @param maxY Maximum Y coordinate of the area.
                                             * @param maxZ Maximum Z coordinate of the area.
                                             * @param boundaryBlockState The block state to use for the boundary.
                                             * @param insideBlockState The block state to use for the inside.
                                             * @param existingOnly If true, only replace existing blocks.
                                             */
    void fillWithBlocks(World &worldIn, const BoundingBox &bbIn, int minX, int minY, int minZ, int maxX, int maxY,
                        int maxZ, lce::BlockState boundaryBlockState,
                        lce::BlockState insideBlockState, bool existingOnly) const;

    /**
     * @brief Fills a specified area within a bounding box with a single block state.
     *
     * @param worldIn The world where the blocks will be placed.
     * @param bbIn The bounding box defining the area to fill.
     * @param minX Minimum X coordinate of the area.
     * @param minY Minimum Y coordinate of the area.
     * @param minZ Minimum Z coordinate of the area.
     * @param maxX Maximum X coordinate of the area.
     * @param maxY Maximum Y coordinate of the area.
     * @param maxZ Maximum Z coordinate of the area.
     * @param theBlockState The block state to use for filling.
     * @param existingOnly If true, only replace existing blocks.
     */
    void fillWithBlocks(World &worldIn, const BoundingBox &bbIn, int minX, int minY, int minZ, int maxX, int maxY,
                        int maxZ, lce::BlockState theBlockState, bool existingOnly) const;

    /**
     * @brief Fills a specified area within a bounding box with air blocks.
     *
     * @param worldIn The world where the blocks will be placed.
     * @param bbIn The bounding box defining the area to fill.
     * @param minX Minimum X coordinate of the area.
     * @param minY Minimum Y coordinate of the area.
     * @param minZ Minimum Z coordinate of the area.
     * @param maxX Maximum X coordinate of the area.
     * @param maxY Maximum Y coordinate of the area.
     * @param maxZ Maximum Z coordinate of the area.
     */
    void fillWithAir(
        World &worldIn, const BoundingBox &bbIn, int minX, int minY, int minZ, int maxX, int maxY, int maxZ) const;

    /**
     * @brief Checks if a bounding box contains any liquid blocks.
     *
     * @param chunkBoundingBoxIn The bounding box to check.
     * @param piece The structure component to compare against.
     * @param chunk The chunk primer to query.
     * @return True if liquid blocks are found, false otherwise.
     */
    static bool isLiquidInStructureBoundingBox(const BoundingBox &chunkBoundingBoxIn,
                                               const StructureComponent &piece, const ChunkPrimer *chunk);

    /**
     * @brief Checks if a specific block position intersects with a bounding box.
     *
     * @param chunkBoundingBox The bounding box to check.
     * @param x The X coordinate of the block.
     * @param y The Y coordinate of the block.
     * @param z The Z coordinate of the block.
     * @return True if the block intersects with the bounding box, false otherwise.
     */
    static bool intersectsWithBlock(const BoundingBox &chunkBoundingBox, int x, int y, int z);

    /**
     * @brief Validates if a block can be placed at a specific position within a bounding box.
     *
     * @param chunkBoundingBox The bounding box to check.
     * @param bb The bounding box of the block to place.
     * @param x The X coordinate of the block.
     * @param y The Y coordinate of the block.
     * @param z The Z coordinate of the block.
     * @return True if the block is valid to place, false otherwise.
     */
    MU static bool validToPlace(const BoundingBox &chunkBoundingBox, const BoundingBox &bb, int x, int y, int z);

    /**
     * @brief Fills a specified area within a bounding box with randomized blocks using only RNG.
     *
     * @param chunkBoundingBox The bounding box defining the area to fill.
     * @param piece The structure component to use for reference.
     * @param minX Minimum X coordinate of the area.
     * @param minY Minimum Y coordinate of the area.
     * @param minZ Minimum Z coordinate of the area.
     * @param maxX Maximum X coordinate of the area.
     * @param maxY Maximum Y coordinate of the area.
     * @param maxZ Maximum Z coordinate of the area.
     * @param rng The random number generator to use.
     * @param chunk The chunk primer to query.
     */
    static void fillWithRandomizedBlocksRNGOnly(const BoundingBox &chunkBoundingBox, const StructureComponent &piece,
                                                int minX, int minY,
                                                int minZ, int maxX, int maxY, int maxZ, RNG &rng,
                                                const ChunkPrimer *chunk);

    /**
     * @brief Generates a chest at a specific position within a bounding box.
     *
     * @param chunkBB The bounding box of the chunk.
     * @param piece The structure component to use for reference.
     * @param rng The random number generator to use.
     * @param x The X coordinate of the chest.
     * @param y The Y coordinate of the chest.
     * @param z The Z coordinate of the chest.
     */
    static void generateChest(const BoundingBox &chunkBB, const StructureComponent &piece, RNG &rng, int x, int y,
                              int z);

    /**
     * @brief Checks if a bounding box contains any liquid blocks in the world.
     *
     * @param worldIn The world to query.
     * @param bbIn The bounding box to check.
     * @return True if liquid blocks are found, false otherwise.
     */
    bool isLiquidInStructureBoundingBox(World &worldIn, const BoundingBox &bbIn) const;

    /**
     * @brief Fills a specified area within a bounding box with blocks, with a rare chance of skipping placement.
     *
     * @param worldIn The world where the blocks will be placed.
     * @param bbIn The bounding box defining the area to fill.
     * @param minX Minimum X coordinate of the area.
     * @param minY Minimum Y coordinate of the area.
     * @param minZ Minimum Z coordinate of the area.
     * @param maxX Maximum X coordinate of the area.
     * @param maxY Maximum Y coordinate of the area.
     * @param maxZ Maximum Z coordinate of the area.
     * @param blockStateIn The block state to use for filling.
     * @param excludeAir If true, air blocks will not be replaced.
     */
    void randomlyRareFillWithBlocks(World &worldIn, const BoundingBox &bbIn, int minX, int minY, int minZ, int maxX,
                                    int maxY, int maxZ, lce::BlockState blockStateIn, bool excludeAir) const;

    /**
     * @brief Retrieves the light level at a specific block position within a bounding box.
     *
     * @param world The world to query.
     * @param x The X coordinate of the block.
     * @param y The Y coordinate of the block.
     * @param z The Z coordinate of the block.
     * @param structureBB The bounding box of the structure.
     * @return The light level at the specified block position.
     */
    int getLightLevelAtBlock(MU World &world, MU int x, MU int y, MU int z, MU const BoundingBox &structureBB) const;

    /**
     * @brief Places a block at a specific position within a bounding box with a given chance.
     *
     * @param worldIn The world where the block will be placed.
     * @param bbIn The bounding box defining the area.
     * @param rand The random number generator to use.
     * @param chance The chance of placing the block.
     * @param x The X coordinate of the block.
     * @param y The Y coordinate of the block.
     * @param z The Z coordinate of the block.
     * @param blockStateIn The block state to place.
     */
    void randomlyPlaceBlock(World &worldIn, const BoundingBox &bbIn, RNG &rand, float chance, int x, int y, int z,
                            lce::BlockState blockStateIn) const;

    /**
     * @brief Fills a specified area within a bounding box with blocks, checking light levels and applying randomization.
     *
     * @param world The world where the blocks will be placed.
     * @param structureBB The bounding box defining the area to fill.
     * @param rng The random number generator to use.
     * @param chance The chance of placing a block.
     * @param minX Minimum X coordinate of the area.
     * @param minY Minimum Y coordinate of the area.
     * @param minZ Minimum Z coordinate of the area.
     * @param maxX Maximum X coordinate of the area.
     * @param maxY Maximum Y coordinate of the area.
     * @param maxZ Maximum Z coordinate of the area.
     * @param blockState1 The first block state to use for filling.
     * @param blockState2 The second block state to use for filling.
     * @param flag If true, apply additional conditions during placement.
     * @param lightLevel The required light level for placement.
     */
    void fillWithBlocksRandomLightCheck(World &world, const BoundingBox &structureBB, RNG &rng, float chance, int minX,
                                        int minY, int minZ, int maxX, int maxY, int maxZ,
                                        lce::BlockState blockState1, lce::BlockState blockState2,
                                        bool flag,
                                        int lightLevel) const;

    /**
     * @brief Fills a specified area within a bounding box with randomized stronghold stones.
     *
     * @param worldIn The world where the blocks will be placed.
     * @param structureBB The bounding box defining the area to fill.
     * @param minX Minimum X coordinate of the area.
     * @param minY Minimum Y coordinate of the area.
     * @param minZ Minimum Z coordinate of the area.
     * @param maxX Maximum X coordinate of the area.
     * @param maxY Maximum Y coordinate of the area.
     * @param maxZ Maximum Z coordinate of the area.
     * @param alwaysReplace If true, always replace existing blocks.
     * @param rng The random number generator to use.
     */
    void fillWithRandomizedStrongholdStones(World &worldIn, const BoundingBox &structureBB, int minX, int minY,
                                            int minZ, int maxX, int maxY, int maxZ, bool alwaysReplace,
                                            RNG &rng) const;

    /**
     * @brief Outputs the structure component to an output stream.
     *
     * @param out The output stream.
     * @param structureComponent The structure component to output.
     * @return The output stream.
     */
    friend std::ostream &operator<<(std::ostream &out, const StructureComponent &structureComponent);
};
