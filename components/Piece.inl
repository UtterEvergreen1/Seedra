#pragma once

inline Pos2D Piece::getBoundingBoxCenter() const {
    return {m_minX + m_maxX / 2, m_minZ + m_maxZ / 2};
}

// getXWithOffset
inline int Piece::getWorldX(c_int offsetWidth, c_int offsetDepth) const {
    switch (m_facing) {
        case EnumFacing::NORTH:
        case EnumFacing::SOUTH:
            return m_minX + offsetWidth;
        case EnumFacing::WEST:
            return m_maxX - offsetDepth;
        case EnumFacing::EAST:
            return m_minX + offsetDepth;
        default:
        case EnumFacing::UP:
        case EnumFacing::DOWN:
            return m_minX; // needed by ocean monument?
    }
}

// getYWithOffset
inline int Piece::getWorldY(c_int offsetHeight) const {
    return m_minY + offsetHeight;
}

// getZWithOffset
inline int Piece::getWorldZ(c_int offsetWidth, c_int offsetDepth) const {
    switch (m_facing) {
        case EnumFacing::NORTH:
            return m_maxZ - offsetDepth;
        case EnumFacing::SOUTH:
            return m_minZ + offsetDepth;
        case EnumFacing::WEST:
        case EnumFacing::EAST:
            return m_minZ + offsetWidth;
        default:
        case EnumFacing::UP:
        case EnumFacing::DOWN:
            std::unreachable();
    }
}

inline  Pos3D Piece::getWorldXYZ(c_int offsetWidth, c_int offsetHeight, c_int offsetDepth) const {
    return {getWorldX(offsetWidth, offsetDepth), getWorldY(offsetHeight), getWorldZ(offsetWidth, offsetDepth)};
}


inline Pos2D Piece::getWorldPos(c_int offsetWidth, c_int offsetDepth) const {
    return {getWorldX(offsetWidth, offsetDepth), getWorldZ(offsetWidth, offsetDepth)};
}

inline BoundingBox Piece::makeBoundingBox(c_int x, c_int y, c_int z, const EnumFacing direction,
                                   c_int width, c_int height, c_int depth) {
    if (direction == EnumFacing::NORTH || direction == EnumFacing::SOUTH) {
        return {
            static_cast<bbType_t>(x),
            static_cast<bbType_t>(y),
            static_cast<bbType_t>(z),
            static_cast<bbType_t>(x + width - 1),
            static_cast<bbType_t>(y + height - 1),
            static_cast<bbType_t>(z + depth - 1)
        };
    }
    return {
        static_cast<bbType_t>(x),
        static_cast<bbType_t>(y),
        static_cast<bbType_t>(z),
        static_cast<bbType_t>(x + depth - 1),
        static_cast<bbType_t>(y + height - 1),
        static_cast<bbType_t>(z + width - 1)
    };
}
