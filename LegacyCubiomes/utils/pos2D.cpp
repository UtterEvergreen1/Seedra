#include "pos2D.hpp"


bool Pos2D::operator==(const Pos2D& other) const {
    return x == other.x && z == other.z;
}


bool Pos2D::operator==(int other) const {
    return x == other && z == other;
}


bool Pos2D::operator!=(const Pos2D& other) const {
    return !(*this == other);
}


bool Pos2D::operator!=(int other) const {
    return !(*this == other);
}


Pos2D Pos2D::operator+(const Pos2D& other) const {
    return { x + other.x, z + other.z };
}


Pos2D Pos2D::operator+(const int other) const {
    return { x + other, z + other };
}


Pos2D Pos2D::operator-(const Pos2D& other) const {
    return { x - other.x, z - other.z };
}


Pos2D Pos2D::operator-(const int other) const {
    return { x - other, z - other };
}


bool Pos2D::operator>(int value) const {
    return x > value && z > value;
}


bool Pos2D::operator<(int value) const {
    return x < value && z < value;
}


bool Pos2D::operator>=(int value) const {
    return x >= value && z >= value;
}


bool Pos2D::operator<=(int value) const {
    return x <= value && z <= value;
}


Pos2D Pos2D::operator>>(int shiftAmount) const {
    return {x >> shiftAmount, z >> shiftAmount};
}


Pos2D Pos2D::operator<<(int shiftAmount) const {
    return {x << shiftAmount, z << shiftAmount};
}


std::string Pos2D::toString() {
    return "(" +
    std::to_string(this->x) +
    ", " +
    std::to_string(this->z) +
    ")";
}


bool Pos2D::insideBounds(int lower, int upper) {
    return *this > lower && *this < upper;
}

