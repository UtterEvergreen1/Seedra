#include "Pos2DTemplate.hpp"

#include "components/BoundingBox.hpp"

template<class classType>
bool Pos2DTemplate<classType>::operator==(const Pos2DTemplate &other) const { return x == other.x && z == other.z; }


template<class classType>
bool Pos2DTemplate<classType>::operator==(int other) const { return x == other && z == other; }


template<class classType>
bool Pos2DTemplate<classType>::operator!=(const Pos2DTemplate &other) const { return x != other.x || z != other.z; }

template<class classType>
bool Pos2DTemplate<classType>::operator!=(int other) const { return x != other || z != other; }


template<class classType>
Pos2DTemplate<classType> Pos2DTemplate<classType>::operator+(const Pos2DTemplate &other) const {
    return {x + other.x, z + other.z};
}


template<class classType>
Pos2DTemplate<classType> Pos2DTemplate<classType>::operator+(c_int other) const { return {x + other, z + other}; }


template<class classType>
Pos2DTemplate<classType> Pos2DTemplate<classType>::operator-(const Pos2DTemplate &other) const {
    return {x - other.x, z - other.z};
}


template<class classType>
Pos2DTemplate<classType> Pos2DTemplate<classType>::operator-(c_int other) const { return {x - other, z - other}; }


template<class classType>
bool Pos2DTemplate<classType>::operator>(classType value) const { return x > value && z > value; }


template<class classType>
bool Pos2DTemplate<classType>::operator<(classType value) const { return x < value && z < value; }


template<class classType>
bool Pos2DTemplate<classType>::operator<(const Pos2DTemplate& other) const {
    if (x < other.x) return true;
    if (x > other.x) return false;
    return z < other.z;
}


template<class classType>
bool Pos2DTemplate<classType>::operator>=(classType value) const { return x >= value && z >= value; }


template<class classType>
bool Pos2DTemplate<classType>::operator<=(classType value) const { return x <= value && z <= value; }


template<class classType>
std::string Pos2DTemplate<classType>::toString() const {
    return "(" + std::to_string(this->x) + ", " + std::to_string(this->z) + ")";
}


template<class classType>
bool Pos2DTemplate<classType>::insideBounds(classType lowerX, classType lowerZ, classType upperX, classType upperZ) const {
    return x >= lowerX && x <= upperX && z >= lowerZ && z <= upperZ;
}

template<class classType>
bool Pos2DTemplate<classType>::insideBounds(const BoundingBox& bb) const {
    return insideBounds(bb.minX, bb.minZ, bb.maxX, bb.maxZ);
}


template<class classType>
void Pos2DTemplate<classType>::setPos(classType xIn, classType zIn) {
    this->x = xIn;
    this->z = zIn;
}

template
class Pos2DTemplate<int>;

template
class Pos2DTemplate<double>;