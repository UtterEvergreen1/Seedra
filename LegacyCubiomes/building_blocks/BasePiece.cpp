#include "BasePiece.hpp"

BasePiece::BasePiece()
        : boundingBox(0, 0, 0, 0, 0, 0), orientation(DIRECTION::NORTH) {
}

BasePiece::BasePiece(BoundingBox boundingBox, DIRECTION orientation)
        : boundingBox(boundingBox), orientation(orientation) {
}

bool BasePiece::operator==(const BasePiece &other) const {
    return boundingBox == other.boundingBox;
}

Pos2D BasePiece::getBoundingBoxCenter() const {
    Pos2D pos{};
    pos.x = this->boundingBox.minX + this->boundingBox.maxX / 2;
    pos.z = this->boundingBox.minZ + this->boundingBox.maxZ / 2;
    return pos;
}


int BasePiece::getWorldX(int offsetWidth, int offsetDepth) const {
    switch (this->orientation) {
        case DIRECTION::NORTH:
        case DIRECTION::SOUTH: {
            return this->boundingBox.minX + offsetWidth;
        }
        case DIRECTION::WEST: {
            return this->boundingBox.maxX - offsetDepth;
        }
        default:
        case DIRECTION::EAST: {
            return this->boundingBox.minX + offsetDepth;
        }
    }
}

[[maybe_unused]] int BasePiece::getWorldY(int offsetHeight) const {
    return this->boundingBox.minY + offsetHeight;
}

int BasePiece::getWorldZ(int offsetWidth, int offsetDepth) const {
    switch (this->orientation) {
        case DIRECTION::NORTH: {
            return this->boundingBox.maxZ - offsetDepth;
        }
        case DIRECTION::SOUTH: {
            return this->boundingBox.minZ + offsetDepth;
        }
        default:
        case DIRECTION::WEST:
        case DIRECTION::EAST: {
            return this->boundingBox.minZ + offsetWidth;
        }
    }
}

Pos2D BasePiece::getWorldPos(int offsetWidth, int offsetDepth) const {
    return {getWorldX(offsetWidth, offsetDepth), getWorldZ(offsetWidth, offsetDepth)};
}

[[maybe_unused]] BoundingBox BasePiece::makeBoundingBox(int x, int y, int z,
                                                        DIRECTION direction,
                                                        int width, int height, int depth) {
    if (direction == DIRECTION::NORTH || direction == DIRECTION::SOUTH) {
        return {x, y, z, x + width - 1, y + height - 1, z + depth - 1};
    } else {
        return {x, y, z, x + depth - 1, y + height - 1, z + width - 1};
    }
}
