#include "Pos2DTemplate.hpp"


template<class classType>
bool Pos2DTemplate<classType>::operator==(const Pos2DTemplate& other) const { return x == other.x && z == other.z; }


template<class classType>
bool Pos2DTemplate<classType>::operator==(int other) const { return x == other && z == other; }


template<class classType>
bool Pos2DTemplate<classType>::operator!=(const Pos2DTemplate& other) const { return *this != other; }

template<class classType>
bool Pos2DTemplate<classType>::operator!=(int other) const { return x != other && z != other; }


template<class classType>
Pos2DTemplate<classType> Pos2DTemplate<classType>::operator+(const Pos2DTemplate& other) const { return {x + other.x, z + other.z}; }


template<class classType>
Pos2DTemplate<classType> Pos2DTemplate<classType>::operator+(const int other) const { return {x + other, z + other}; }


template<class classType>
Pos2DTemplate<classType> Pos2DTemplate<classType>::operator-(const Pos2DTemplate& other) const { return {x - other.x, z - other.z}; }


template<class classType>
Pos2DTemplate<classType> Pos2DTemplate<classType>::operator-(const int other) const { return {x - other, z - other}; }


template<class classType>
bool Pos2DTemplate<classType>::operator>(classType value) const { return x > value && z > value; }


template<class classType>
bool Pos2DTemplate<classType>::operator<(classType value) const { return x < value && z < value; }


template<class classType>
bool Pos2DTemplate<classType>::operator>=(classType value) const { return x >= value && z >= value; }


template<class classType>
bool Pos2DTemplate<classType>::operator<=(classType value) const { return x <= value && z <= value; }


template<class classType>
std::string Pos2DTemplate<classType>::toString() { return "(" + std::to_string(this->x) + ", " + std::to_string(this->z) + ")"; }


template<class classType>
bool Pos2DTemplate<classType>::insideBounds(classType lower, classType upper) { return *this > lower && *this < upper; }


template<class classType>
void Pos2DTemplate<classType>::setPos(classType xIn, classType zIn) {
    this->x = xIn;
    this->z = zIn;
}

template class Pos2DTemplate<int>;
template class Pos2DTemplate<double>;