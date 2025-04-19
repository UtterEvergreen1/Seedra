#pragma once

#include "lce/processor.hpp"
#include "common/Pos2DTemplate.hpp"
#include "BoundingBox.hpp"

#include "PieceType.hpp"

/**
 * @class Piece
 * @brief Represents a structural piece with a bounding box, orientation, type, and additional metadata.
 *
 * This class extends the BoundingBox class and provides additional properties and methods
 * for handling structural pieces, such as their orientation, type, depth, and associated data.
 */
class Piece : public BoundingBox {
public:
    EnumFacing facing; ///< The orientation of the piece.
    PieceType type; ///< The type of the piece (e.g., NONE, specific structure types).
    i8 depth; ///< The depth of the piece in the structure hierarchy.
    i32 data; ///< Additional data associated with the piece.

    /**
     * @brief Default constructor initializing a piece with default values.
     */
    Piece() : facing(), type(PT_NONE), depth(0), data(0) {
    }

    /**
     * @brief Constructs a piece with a bounding box and orientation.
     *
     * @param bb The bounding box of the piece.
     * @param facing The orientation of the piece.
     */
    Piece(const BoundingBox bb, const EnumFacing facing)
        : BoundingBox(bb), facing(facing), type(PT_NONE), depth(0), data(0) {
    }

    /**
     * @brief Constructs a piece with specified bounding box coordinates and orientation.
     *
     * @param minX Minimum X coordinate of the bounding box.
     * @param minY Minimum Y coordinate of the bounding box.
     * @param minZ Minimum Z coordinate of the bounding box.
     * @param maxX Maximum X coordinate of the bounding box.
     * @param maxY Maximum Y coordinate of the bounding box.
     * @param maxZ Maximum Z coordinate of the bounding box.
     * @param facing The orientation of the piece.
     */
    Piece(c_int minX, c_int minY, c_int minZ, c_int maxX, c_int maxY, c_int maxZ, const EnumFacing facing)
        : BoundingBox(minX, minY, minZ, maxX, maxY, maxZ), facing(facing), type(PT_NONE), depth(0), data(0) {
    }

    /**
     * @brief Constructs a piece with specified type, depth, bounding box, orientation, and data.
     *
     * @param type The type of the piece.
     * @param depth The depth of the piece in the structure hierarchy.
     * @param bb The bounding box of the piece.
     * @param facing The orientation of the piece.
     * @param data Additional data associated with the piece.
     */
    Piece(const PieceType type, c_i8 depth, const BoundingBox &bb, const EnumFacing facing, c_int data)
        : BoundingBox(bb), facing(facing), type(type), depth(depth), data(data) {
    }

    /**
     * @brief Gets the center of the bounding box as a 2D position.
     *
     * @return The center of the bounding box as a Pos2D object.
     */
    MU ND Pos2D getBoundingBoxCenter() const;

    /**
     * @brief Gets the world X coordinate based on an offset.
     *
     * @param offsetWidth The offset in the width dimension.
     * @return The world X coordinate.
     */
    ND int getWorldX(int offsetWidth, int offsetDepth) const;

    /**
     * @brief Gets the world Y coordinate based on an offset.
     *
     * @param offsetHeight The offset in the height dimension.
     * @return The world Y coordinate.
     */
    ND int getWorldY(int offsetHeight) const;

    /**
     * @brief Gets the world Z coordinate based on an offset.
     *
     * @param offsetWidth The offset in the width dimension.
     * @param offsetDepth The offset in the depth dimension.
     * @return The world Z coordinate.
     */
    ND int getWorldZ(int offsetWidth, int offsetDepth) const;

    /**
     * @brief Gets the world position as a 3D coordinate based on offsets.
     *
     * @param offsetWidth The offset in the width dimension.
     * @param offsetHeight The offset in the height dimension.
     * @param offsetDepth The offset in the depth dimension.
     * @return The world position as a Pos3D object.
     */
    MU ND Pos3D getWorldXYZ(int offsetWidth, int offsetHeight, int offsetDepth) const;

    /**
     * @brief Gets the world position as a 2D coordinate based on offsets.
     *
     * @param offsetWidth The offset in the width dimension.
     * @param offsetDepth The offset in the depth dimension.
     * @return The world position as a Pos2D object.
     */
    ND MU Pos2D getWorldPos(int offsetWidth, int offsetDepth) const;

    /**
     * @brief Creates a bounding box for the piece based on position, orientation, and dimensions.
     *
     * @param x The X coordinate of the origin.
     * @param y The Y coordinate of the origin.
     * @param z The Z coordinate of the origin.
     * @param direction The orientation of the bounding box.
     * @param width The width of the bounding box.
     * @param height The height of the bounding box.
     * @param depth The depth of the bounding box.
     * @return The created bounding box.
     */
    MU static BoundingBox makeBoundingBox(int x, int y, int z, EnumFacing direction, int width, int height, int depth);

    /**
     * @brief Checks if the piece type is NONE.
     *
     * @return True if the piece type is NONE, false otherwise.
     */
    MU ND bool isTypeNone() const { return type == PT_NONE; }
};

#include "Piece.inl"
