#include "DoublePos3D.hpp"


bool DoublePos3D::operator==(const DoublePos3D& other) const { return x == other.x && z == other.z; }


DoublePos3D DoublePos3D::operator+(const DoublePos3D& other) const { return {x + other.x, y + other.y, z + other.z}; }


DoublePos3D DoublePos3D::operator+(const double other) const { return {x + other, y + other, z + other}; }


DoublePos3D DoublePos3D::operator-(const DoublePos3D& other) const { return {x - other.x, y - other.y, z - other.z}; }


DoublePos3D DoublePos3D::operator-(const double other) const { return {x - other, y - other, z - other}; }


bool DoublePos3D::operator>(double value) const { return x > value && y > value && z > value; }


bool DoublePos3D::operator<(double value) const { return x < value && y < value && z < value; }


bool DoublePos3D::operator>=(double value) const { return x >= value && y >= value && z >= value; }


bool DoublePos3D::operator<=(double value) const { return x <= value && y <= value && z <= value; }


DoublePos2D DoublePos3D::convert2D() { return {x, z}; }


bool DoublePos3D::insideBounds(double lower, double upper) { return *this > lower && *this < upper; }
