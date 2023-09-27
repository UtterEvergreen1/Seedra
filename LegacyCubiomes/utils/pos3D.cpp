#include "pos3D.hpp"


bool Pos3D::operator==(const Pos3D& other) const {
    return x == other.x && z == other.z;
}


Pos3D Pos3D::operator+(const Pos3D& other) const {
    return { x + other.x, y + other.y, z + other.z };
}


Pos3D Pos3D::operator+(const int other) const {
    return { x + other, y + other, z + other };
}


Pos3D Pos3D::operator-(const Pos3D& other) const {
    return { x - other.x, y - other.y, z - other.z };
}


Pos3D Pos3D::operator-(const int other) const {
    return { x - other, y - other, z - other };
}


bool Pos3D::operator>(int value) const {
    return x > value && y > value && z > value;
}


bool Pos3D::operator<(int value) const {
    return x < value && y < value && z < value;
}


bool Pos3D::operator>=(int value) const {
    return x >= value && y >= value && z >= value;
}


bool Pos3D::operator<=(int value) const {
    return x <= value && y <= value && z <= value;
}


Pos3D Pos3D::operator>>(int shiftAmount) const {
    return {x >> shiftAmount, y >> shiftAmount, z >> shiftAmount};
}


Pos3D Pos3D::operator<<(int shiftAmount) const {
    return {x << shiftAmount, y << shiftAmount, z << shiftAmount};
}


Pos2D Pos3D::convert2D() {
    return {x, z};
}


bool Pos3D::insideBounds(int lower, int upper) {
    return *this > lower && *this < upper;
}
