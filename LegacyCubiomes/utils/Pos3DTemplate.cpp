#include "Pos3DTemplate.hpp"


template<class classType>
bool Pos3DTemplate<classType>::operator==(const Pos3DTemplate &other) const { return x == other.x && z == other.z; }


template<class classType>
Pos3DTemplate<classType> Pos3DTemplate<classType>::operator+(const Pos3DTemplate &other) const {
    return {x + other.x, y + other.y, z + other.z};
}


template<class classType>
Pos3DTemplate<classType> Pos3DTemplate<classType>::operator+(const classType other) const {
    return {x + other, y + other, z + other};
}


template<class classType>
Pos3DTemplate<classType> Pos3DTemplate<classType>::operator-(const Pos3DTemplate &other) const {
    return {x - other.x, y - other.y, z - other.z};
}


template<class classType>
Pos3DTemplate<classType> Pos3DTemplate<classType>::operator-(const classType other) const {
    return {x - other, y - other, z - other};
}


template<class classType>
bool Pos3DTemplate<classType>::operator>(int value) const { return x > value && y > value && z > value; }


template<class classType>
bool Pos3DTemplate<classType>::operator<(int value) const { return x < value && y < value && z < value; }


template<class classType>
bool Pos3DTemplate<classType>::operator>=(int value) const { return x >= value && y >= value && z >= value; }


template<class classType>
bool Pos3DTemplate<classType>::operator<=(int value) const { return x <= value && y <= value && z <= value; }

template<class classType>
Pos3DTemplate<classType> Pos3DTemplate<classType>::offset(const FACING facing, const int n) const {
    switch (facing) {
        case FACING::NORTH:
            return {x, y, z - n};
        case FACING::SOUTH:
            return {x, y, z + n};
        case FACING::WEST:
            return {x - n, y, z};
        case FACING::EAST:
            return {x + n, y, z};
        case FACING::UP:
            return {x, y + n, z};
        case FACING::DOWN:
            return {x, y - n, z};
        default:
            return *this;
    }
}

template<class classType>
double Pos3DTemplate<classType>::distanceSq(c_double toX, c_double toY, c_double toZ) const {
    c_double d0 = static_cast<double>(x) - toX;
    c_double d1 = static_cast<double>(y) - toY;
    c_double d2 = static_cast<double>(z) - toZ;
    return d0 * d0 + d1 * d1 + d2 * d2;
}

template<class classType>
Pos2DTemplate<classType> Pos3DTemplate<classType>::convert2D() const { return {x, z}; }

template<class classType>
template<typename, typename>
Pos3DTemplate<classType> Pos3DTemplate<classType>::convertToChunkCords() const {
    return {x & 15, y & 255, z & 15};
}


template<class classType>
bool Pos3DTemplate<classType>::insideBounds(classType lowerX, classType lowerY, classType lowerZ, classType upperX,
                                            classType upperY, classType upperZ) const {
    return x >= lowerX && x <= upperX && y >= lowerY && y <= upperY && z >= lowerZ && z <= upperZ;
}

template<class classType>
std::vector<Pos3DTemplate<classType>> Pos3DTemplate<classType>::getAllInBox(
    const Pos3DTemplate &from, const Pos3DTemplate &to) {
    c_int minX = std::min(from.getX(), to.getX());
    c_int minY = std::min(from.getY(), to.getY());
    c_int minZ = std::min(from.getZ(), to.getZ());
    c_int maxX = std::max(from.getX(), to.getX());
    c_int maxY = std::max(from.getY(), to.getY());
    c_int maxZ = std::max(from.getZ(), to.getZ());

    std::vector<Pos3DTemplate<classType>> positions;
    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            for (int z = minZ; z <= maxZ; ++z) {
                positions.emplace_back(x, y, z);
            }
        }
    }
    return positions;
}

template
class Pos3DTemplate<int>;

template
class Pos3DTemplate<double>;

template Pos3DTemplate<int> Pos3DTemplate<int>::convertToChunkCords<int, void>() const;
