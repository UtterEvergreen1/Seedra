#pragma once

typedef short bbType_t;


inline const BoundingBox BoundingBox::EMPTY =
    BoundingBox(SHRT_MAX, SHRT_MAX, SHRT_MAX, SHRT_MIN, SHRT_MIN, SHRT_MIN);


inline BoundingBox::BoundingBox() = default;


inline BoundingBox::BoundingBox(
    const bbType_t minX, const bbType_t minY, const bbType_t minZ,
    const bbType_t maxX, const bbType_t maxY, const bbType_t maxZ)
    : m_minX(minX), m_minY(minY), m_minZ(minZ), m_maxX(maxX), m_maxY(maxY), m_maxZ(maxZ) {}

inline BoundingBox::BoundingBox(
    const bbType_t minX, const bbType_t minZ,
    const bbType_t maxX, const bbType_t maxZ)
    : BoundingBox(minX, 0, minZ, maxX, 255, maxZ) {}

inline void BoundingBox::setBoundingBox(BoundingBox boundingBox) {
    *this = boundingBox;
}

inline bool BoundingBox::operator==(const BoundingBox& other) const {
    return m_minX == other.m_minX && m_maxX == other.m_maxX &&
           m_minZ == other.m_minZ && m_maxZ == other.m_maxZ &&
           m_minY == other.m_minY && m_maxY == other.m_maxY;
}

inline BoundingBox BoundingBox::operator<<(int shiftAmount) const {
    return {
        static_cast<bbType_t>(m_minX << shiftAmount),
        static_cast<bbType_t>(m_minY << shiftAmount),
        static_cast<bbType_t>(m_minZ << shiftAmount),
        static_cast<bbType_t>(m_maxX << shiftAmount),
        static_cast<bbType_t>(m_maxY << shiftAmount),
        static_cast<bbType_t>(m_maxZ << shiftAmount)
    };
}

inline bool BoundingBox::intersects(const BoundingBox& other) const {
    return m_maxX >= other.m_minX
        && m_minX <= other.m_maxX
        && m_maxZ >= other.m_minZ
        && m_minZ <= other.m_maxZ
        && m_maxY >= other.m_minY
        && m_minY <= other.m_maxY;
}


inline bool BoundingBox::contains(const BoundingBox& other) const {
    return m_maxX >= other.m_maxX && m_minX <= other.m_minX && m_maxY >= other.m_maxY &&
           m_minY <= other.m_minY && m_maxZ >= other.m_maxZ && m_minZ <= other.m_minZ;
}


/// makes "*this" expand such that it contains "other".
inline void BoundingBox::encompass(const BoundingBox& other) {
    if (other.m_minX < m_minX) m_minX = other.m_minX;
    if (other.m_minY < m_minY) m_minY = other.m_minY;
    if (other.m_minZ < m_minZ) m_minZ = other.m_minZ;
    if (other.m_maxX > m_maxX) m_maxX = other.m_maxX;
    if (other.m_maxY > m_maxY) m_maxY = other.m_maxY;
    if (other.m_maxZ > m_maxZ) m_maxZ = other.m_maxZ;
}


/// makes "*this" expand such that it contains "other". Only y-coords.
inline void BoundingBox::encompassY(const BoundingBox& other) {
    if (other.m_minY < m_minY) m_minY = other.m_minY;
    if (other.m_maxY > m_maxY) m_maxY = other.m_maxY;
}


/// makes "*this" shrink such that "other" contains it.
inline void BoundingBox::shrinkToFit(const BoundingBox& other) {
    if (m_minX < other.m_minX) m_minX = other.m_minX;
    if (m_minY < other.m_minY) m_minY = other.m_minY;
    if (m_minZ < other.m_minZ) m_minZ = other.m_minZ;
    if (m_maxX > other.m_maxX) m_maxX = other.m_maxX;
    if (m_maxY > other.m_maxY) m_maxY = other.m_maxY;
    if (m_maxZ > other.m_maxZ) m_maxZ = other.m_maxZ;
}


inline bool BoundingBox::isVecInside(const Pos3D& pos) const {
    return this->m_maxX >= pos.x && this->m_minX <= pos.x &&
           this->m_maxY >= pos.y && this->m_minY <= pos.y &&
           this->m_maxZ >= pos.z && this->m_minZ <= pos.z;
}

inline bool BoundingBox::isVecInside(const Pos2D& pos) const {
    return this->m_maxX >= pos.x && this->m_minX <= pos.x &&
           this->m_maxZ >= pos.z && this->m_minZ <= pos.z;
}


inline void BoundingBox::offset(c_int x, c_int y, c_int z) {
    m_minX += static_cast<bbType_t>(x);
    m_minY += static_cast<bbType_t>(y);
    m_minZ += static_cast<bbType_t>(z);
    m_maxX += static_cast<bbType_t>(x);
    m_maxY += static_cast<bbType_t>(y);
    m_maxZ += static_cast<bbType_t>(z);
}


inline void BoundingBox::offsetY(c_int y) {
    m_minY += static_cast<bbType_t>(y);
    m_maxY += static_cast<bbType_t>(y);
}


inline BoundingBox BoundingBox::makeChunkBox(c_int xChunk, c_int zChunk) {
    return {
        static_cast<bbType_t>(xChunk << 4),
        static_cast<bbType_t>(0),
        static_cast<bbType_t>(zChunk << 4),
        static_cast<bbType_t>((xChunk << 4) + 15),
        static_cast<bbType_t>(255),
        static_cast<bbType_t>((zChunk << 4) + 15)
    };
}


inline BoundingBox BoundingBox::orientBox(c_int x, c_int y, c_int z,
                                   c_int width, c_int height,
                                   c_int depth, const EnumFacing direction) {
    switch (direction) {
        case EnumFacing::NORTH:
            return {
                static_cast<bbType_t>(x),
                static_cast<bbType_t>(y),
                static_cast<bbType_t>(z - depth + 1),
                static_cast<bbType_t>(x + width - 1),
                static_cast<bbType_t>(y + height - 1),
                static_cast<bbType_t>(z)
            };
        case EnumFacing::SOUTH:
            return {
                static_cast<bbType_t>(x),
                static_cast<bbType_t>(y),
                static_cast<bbType_t>(z),
                static_cast<bbType_t>(x + width - 1),
                static_cast<bbType_t>(y + height - 1),
                static_cast<bbType_t>(z + depth - 1)
            };
        case EnumFacing::WEST:
            return {
                static_cast<bbType_t>(x - depth + 1),
                static_cast<bbType_t>(y),
                static_cast<bbType_t>(z),
                static_cast<bbType_t>(x),
                static_cast<bbType_t>(y + height - 1),
                static_cast<bbType_t>(z + width - 1)
            };
        default:
        case EnumFacing::EAST:
            return {
                static_cast<bbType_t>(x),
                static_cast<bbType_t>(y),
                static_cast<bbType_t>(z),
                static_cast<bbType_t>(x + depth - 1),
                static_cast<bbType_t>(y + height - 1),
                static_cast<bbType_t>(z + width - 1)
            };
        case EnumFacing::DOWN:
        case EnumFacing::UP:
            std::unreachable();
    }
}


inline BoundingBox BoundingBox::orientBox(const Pos3D& xyz, c_int width, c_int height, c_int depth, const EnumFacing direction) {
    switch (direction) {
        case EnumFacing::NORTH:
            return {
                static_cast<bbType_t>(xyz.getX()),
                static_cast<bbType_t>(xyz.getY()),
                static_cast<bbType_t>(xyz.getZ() - depth + 1),
                static_cast<bbType_t>(xyz.getX() + width - 1),
                static_cast<bbType_t>(xyz.getY() + height - 1),
                static_cast<bbType_t>(xyz.getZ())
            };
        case EnumFacing::SOUTH:
            return {
                static_cast<bbType_t>(xyz.getX()),
                static_cast<bbType_t>(xyz.getY()),
                static_cast<bbType_t>(xyz.getZ()),
                static_cast<bbType_t>(xyz.getX() + width - 1),
                static_cast<bbType_t>(xyz.getY() + height - 1),
                static_cast<bbType_t>(xyz.getZ() + depth - 1)
            };
        case EnumFacing::WEST:
            return {
                static_cast<bbType_t>(xyz.getX() - depth + 1),
                static_cast<bbType_t>(xyz.getY()),
                static_cast<bbType_t>(xyz.getZ()),
                static_cast<bbType_t>(xyz.getX()),
                static_cast<bbType_t>(xyz.getY() + height - 1),
                static_cast<bbType_t>(xyz.getZ() + width - 1)
            };
        default:
        case EnumFacing::EAST:
            return {
                static_cast<bbType_t>(xyz.getX()),
                static_cast<bbType_t>(xyz.getY()),
                static_cast<bbType_t>(xyz.getZ()),
                static_cast<bbType_t>(xyz.getX() + depth - 1),
                static_cast<bbType_t>(xyz.getY() + height - 1),
                static_cast<bbType_t>(xyz.getZ() + width - 1)
            };
        case EnumFacing::DOWN:
        case EnumFacing::UP:
            std::unreachable();
    }
}


inline BoundingBox BoundingBox::orientBox(
    c_int x, c_int y, c_int z,
    c_int offsetWidth, c_int offsetHeight, c_int offsetDepth,
    c_int width, c_int height, c_int depth,
    const EnumFacing direction
) {
    switch (direction) {
        case EnumFacing::NORTH:
            return {
                static_cast<bbType_t>(x + offsetWidth),
                static_cast<bbType_t>(y + offsetHeight),
                static_cast<bbType_t>(z - depth + 1 + offsetDepth),
                static_cast<bbType_t>(x + width - 1 + offsetWidth),
                static_cast<bbType_t>(y + height - 1 + offsetHeight),
                static_cast<bbType_t>(z + offsetDepth)
            };
        case EnumFacing::SOUTH:
            return {
                static_cast<bbType_t>(x + offsetWidth),
                static_cast<bbType_t>(y + offsetHeight),
                static_cast<bbType_t>(z + offsetDepth),
                static_cast<bbType_t>(x + width - 1 + offsetWidth),
                static_cast<bbType_t>(y + height - 1 + offsetHeight),
                static_cast<bbType_t>(z + depth - 1 + offsetDepth)
            };
        case EnumFacing::WEST:
            return {
                static_cast<bbType_t>(x - depth + 1 + offsetDepth),
                static_cast<bbType_t>(y + offsetHeight),
                static_cast<bbType_t>(z + offsetWidth),
                static_cast<bbType_t>(x + offsetDepth),
                static_cast<bbType_t>(y + height - 1 + offsetHeight),
                static_cast<bbType_t>(z + width - 1 + offsetWidth)
            };
        default:
        case EnumFacing::EAST:
            return {
                static_cast<bbType_t>(x + offsetDepth),
                static_cast<bbType_t>(y + offsetHeight),
                static_cast<bbType_t>(z + offsetWidth),
                static_cast<bbType_t>(x + depth - 1 + offsetDepth),
                static_cast<bbType_t>(y + height - 1 + offsetHeight),
                static_cast<bbType_t>(z + width - 1 + offsetWidth)
            };
        case EnumFacing::DOWN:
        case EnumFacing::UP:
            std::unreachable();
    }
}


inline BoundingBox BoundingBox::orientBox(const Pos3D& posXYZ, const Pos3D& posOffset, const Pos3D& size,
                                   const EnumFacing direction) {
    return orientBox(
        posXYZ.getX(), posXYZ.getY(), posXYZ.getZ(),
        posOffset.getX(), posOffset.getY(), posOffset.getZ(),
        size.getX(), size.getY(), size.getZ(),
        direction
    );
}

inline BoundingBox BoundingBox::orientBox(const Pos3D& posXYZ, c_int offsetWidth, c_int offsetHeight,
    c_int offsetDepth, c_int width, c_int height, c_int depth, const EnumFacing direction) {
    return orientBox(
        posXYZ.getX(), posXYZ.getY(), posXYZ.getZ(),
        offsetWidth, offsetHeight, offsetDepth,
        width, height, depth,
        direction
    );
}

// used by ocean monument
inline BoundingBox BoundingBox::createProper(int x1, int y1, int z1, int x2, int y2, int z2) {
    return {
        static_cast<bbType_t>(std::min(x1, x2)),
        static_cast<bbType_t>(std::min(y1, y2)),
        static_cast<bbType_t>(std::min(z1, z2)),
        static_cast<bbType_t>(std::max(x1, x2)),
        static_cast<bbType_t>(std::max(y1, y2)),
        static_cast<bbType_t>(std::max(z1, z2))
    };
}

inline std::string BoundingBox::toString() const {
    return "(" + std::to_string(m_minX) + ", " + std::to_string(m_minY) + ", " + std::to_string(m_minZ) + ") -> (" +
           std::to_string(m_maxX) + ", " + std::to_string(m_maxY) + ", " + std::to_string(m_maxZ) + ")";
}


inline std::ostream& operator<<(std::ostream& out, const BoundingBox& boundingBox) {
    out << boundingBox.toString();
    return out;
}
