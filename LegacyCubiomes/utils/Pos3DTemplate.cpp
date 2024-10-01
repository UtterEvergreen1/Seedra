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
Pos2DTemplate<classType> Pos3DTemplate<classType>::convert2D() const { return {x, z}; }


template<class classType>
bool Pos3DTemplate<classType>::insideBounds(classType lowerX, classType lowerY, classType lowerZ, classType upperX,
                                            classType upperY, classType upperZ) const {
    return x >= lowerX && x <= upperX && y >= lowerY && y <= upperY && z >= lowerZ && z <= upperZ;
}

template
class Pos3DTemplate<int>;

template
class Pos3DTemplate<double>;