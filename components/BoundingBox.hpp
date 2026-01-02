#pragma once

#include "lce/enums.hpp"
#include "lce/processor.hpp"
#include "common/Pos3DTemplate.hpp"

typedef short bbType_t;

/**
 * @class BoundingBox
 * @brief Represents a 3D bounding box with utility methods for manipulation and intersection checks.
 */
class BoundingBox {
public:

    /// A constant representing an empty bounding box.
    static const BoundingBox EMPTY;

    /// Minimum and maximum coordinates of the bounding box.
    bbType_t m_minX{}, m_minY{}, m_minZ{}, m_maxX{}, m_maxY{}, m_maxZ{};

    /**
     * @brief Default constructor initializing an empty bounding box.
     */
    BoundingBox();

    /**
     * @brief Constructs a bounding box with specified minimum and maximum coordinates.
     *
     * @param minX Minimum X coordinate.
     * @param minY Minimum Y coordinate.
     * @param minZ Minimum Z coordinate.
     * @param maxX Maximum X coordinate.
     * @param maxY Maximum Y coordinate.
     * @param maxZ Maximum Z coordinate.
     */
    BoundingBox(bbType_t minX, bbType_t minY, bbType_t minZ, bbType_t maxX, bbType_t maxY, bbType_t maxZ);

    /**
     * @brief Constructs a 2D bounding box with specified minimum and maximum X and Z coordinates.
     *
     * @param minX Minimum X coordinate.
     * @param minZ Minimum Z coordinate.
     * @param maxX Maximum X coordinate.
     * @param maxZ Maximum Z coordinate.
     */
    BoundingBox(bbType_t minX, bbType_t minZ, bbType_t maxX, bbType_t maxZ);

    /**
     * @brief Equality operator to compare two bounding boxes.
     *
     * @param other The other bounding box to compare.
     * @return True if the bounding boxes are equal, false otherwise.
     */
    bool operator==(const BoundingBox &other) const;

    /**
     * @brief Shifts the bounding box by a specified amount.
     *
     * @param shiftAmount The amount to shift.
     * @return A new bounding box shifted by the specified amount.
     */
    BoundingBox operator<<(int shiftAmount) const;

    /**
     * @brief Shifts the bounding box by a specified amount in the opposite direction.
     *
     * @param shiftAmount The amount to shift.
     * @return A new bounding box shifted by the specified amount in the opposite direction.
     */
    BoundingBox operator>>(int shiftAmount) const {
        return {
            static_cast<bbType_t>(m_minX >> shiftAmount),
            static_cast<bbType_t>(m_minY >> shiftAmount),
            static_cast<bbType_t>(m_minZ >> shiftAmount),
            static_cast<bbType_t>(m_maxX >> shiftAmount),
            static_cast<bbType_t>(m_maxY >> shiftAmount),
            static_cast<bbType_t>(m_maxZ >> shiftAmount)
        };
    }

    void setBoundingBox(BoundingBox boundingBox);

    /**
     * @brief Checks if this bounding box intersects with another bounding box.
     *
     * @param other The other bounding box to check.
     * @return True if the bounding boxes intersect, false otherwise.
     */
    ND bool intersects(const BoundingBox &other) const;

    /**
     * @brief Checks if this bounding box completely contains another bounding box.
     *
     * @param other The other bounding box to check.
     * @return True if this bounding box contains the other, false otherwise.
     */
    MU ND bool contains(const BoundingBox &other) const;

    /**
     * @brief Expands this bounding box to encompass another bounding box.
     *
     * @param other The other bounding box to encompass.
     */
    MU void encompass(const BoundingBox &other);

    /**
     * @brief Expands this bounding box in the Y dimension to encompass another bounding box.
     *
     * @param other The other bounding box to encompass.
     */
    MU void encompassY(const BoundingBox &other);

    /**
     * @brief Shrinks this bounding box to fit within another bounding box.
     *
     * @param other The other bounding box to fit within.
     */
    MU void shrinkToFit(const BoundingBox &other);

    /**
     * @brief Checks if a 3D position is inside this bounding box.
     *
     * @param pos The 3D position to check.
     * @return True if the position is inside, false otherwise.
     */
    MU ND bool isVecInside(const Pos3D &pos) const;

    /**
     * @brief Checks if a 2D position is inside this bounding box.
     *
     * @param pos The 2D position to check.
     * @return True if the position is inside, false otherwise.
     */
    MU ND bool isVecInside(const Pos2D &pos) const;

    /**
     * @brief Offsets the bounding box by specified amounts in the X, Y, and Z dimensions.
     *
     * @param x Offset in the X dimension.
     * @param y Offset in the Y dimension.
     * @param z Offset in the Z dimension.
     */
    MU void offset(int x, int y, int z);

    /**
     * @brief Offsets the bounding box by a specified amount in the Y dimension.
     *
     * @param y Offset in the Y dimension.
     */
    MU void offsetY(int y);

    /**
     * @brief Gets the size of the bounding box in the X dimension.
     *
     * @return The size in the X dimension.
     */
    MU ND int getXSize() const { return m_maxX - m_minX + 1; }

    /**
     * @brief Gets the size of the bounding box in the Y dimension.
     *
     * @return The size in the Y dimension.
     */
    MU ND int getYSize() const { return m_maxY - m_minY + 1; }

    /**
     * @brief Gets the size of the bounding box in the Z dimension.
     *
     * @return The size in the Z dimension.
     */
    MU ND int getZSize() const { return m_maxZ - m_minZ + 1; }

    /**
     * @brief Gets the maximum length of the bounding box in the X or Z dimension.
     *
     * @return The maximum length.
     */
    MU ND int getLength() const { return std::max(m_maxX - m_minX, m_maxZ - m_minZ); }

    /*
     * @brief Gets the area of the bounding box in the X-Z plane.
     * @return The area of the bounding box.
     */
    MU ND int getArea() const { return getXSize() * getZSize(); }

    /**
     * @brief Gets the center coordinate in the X dimension.
     *
     * @return The center X coordinate.
     */
    MU ND int getCenterX() const { return (m_minX + m_maxX) / 2; }

    /**
     * @brief Gets the center coordinate in the Z dimension.
     *
     * @return The center Z coordinate.
     */
    MU ND int getCenterZ() const { return (m_minZ + m_maxZ) / 2; }

    /**
     * @brief Creates a bounding box for a specific chunk.
     *
     * @param xChunk The X coordinate of the chunk.
     * @param zChunk The Z coordinate of the chunk.
     * @return The created bounding box.
     */
    static BoundingBox makeChunkBox(int xChunk, int zChunk);

    /**
     * @brief Creates an oriented bounding box based on specified dimensions and direction.
     *
     * @param x X coordinate of the origin.
     * @param y Y coordinate of the origin.
     * @param z Z coordinate of the origin.
     * @param offsetWidth Offset in the width dimension.
     * @param offsetHeight Offset in the height dimension.
     * @param offsetDepth Offset in the depth dimension.
     * @param width Width of the box.
     * @param height Height of the box.
     * @param depth Depth of the box.
     * @param direction The direction to orient the box.
     * @return The created bounding box.
     */
    static BoundingBox orientBox(
        int x, int y, int z,
        int offsetWidth, int offsetHeight, int offsetDepth,
        int width, int height, int depth, EnumFacing direction);

    /**
     * @brief Creates an oriented bounding box based on specified dimensions and direction.
     *
     * @param x X coordinate of the origin.
     * @param y Y coordinate of the origin.
     * @param z Z coordinate of the origin.
     * @param width Width of the box.
     * @param height Height of the box.
     * @param depth Depth of the box.
     * @param direction The direction to orient the box.
     * @return The created bounding box.
     */
    MU static BoundingBox orientBox(int x, int y, int z, int width, int height, int depth, EnumFacing direction);

    /**
     * @brief Creates an oriented bounding box based on a position and dimensions.
     *
     * @param xyz The origin position.
     * @param width Width of the box.
     * @param height Height of the box.
     * @param depth Depth of the box.
     * @param direction The direction to orient the box.
     * @return The created bounding box.
     */
    static BoundingBox orientBox(const Pos3D &xyz, int width, int height, int depth, EnumFacing direction);

    /**
     * @brief Creates an oriented bounding box based on a position, offset, and size.
     *
     * @param posXYZ The origin position.
     * @param posOffset The offset position.
     * @param size The size of the box.
     * @param direction The direction to orient the box.
     * @return The created bounding box.
     */
    MU static BoundingBox orientBox(const Pos3D &posXYZ, const Pos3D &posOffset, const Pos3D &size,
                                    EnumFacing direction);

    /**
     * @brief Creates an oriented bounding box based on a position, offsets, and dimensions.
     *
     * @param posXYZ The origin position.
     * @param offsetWidth Offset in the width dimension.
     * @param offsetHeight Offset in the height dimension.
     * @param offsetDepth Offset in the depth dimension.
     * @param width Width of the box.
     * @param height Height of the box.
     * @param depth Depth of the box.
     * @param direction The direction to orient the box.
     * @return The created bounding box.
     */
    static BoundingBox orientBox(const Pos3D &posXYZ, int offsetWidth, int offsetHeight, int offsetDepth, int width,
                                 int height, int depth, EnumFacing direction);

    static BoundingBox createProper(int x1, int y1, int z1, int x2, int y2, int z2);

    /**
     * @brief Converts the bounding box to a string representation.
     *
     * @return The string representation of the bounding box.
     */
    MU ND std::string toString() const;

    /**
     * @brief Outputs the bounding box to an output stream.
     *
     * @param out The output stream.
     * @param boundingBox The bounding box to output.
     * @return The output stream.
     */
    friend std::ostream &operator<<(std::ostream &out, const BoundingBox &boundingBox);
};

#include "BoundingBox.inl"
