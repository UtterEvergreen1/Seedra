#include "DoublePos2D.hpp"


bool DoublePos2D::operator==(const DoublePos2D& other) const { return x == other.x && z == other.z; }


bool DoublePos2D::operator==(double other) const { return x == other && z == other; }


bool DoublePos2D::operator!=(const DoublePos2D& other) const { return !(*this == other); }


bool DoublePos2D::operator!=(double other) const { return !(*this == other); }


DoublePos2D DoublePos2D::operator+(const DoublePos2D& other) const { return {x + other.x, z + other.z}; }


DoublePos2D DoublePos2D::operator+(const double other) const { return {x + other, z + other}; }


DoublePos2D DoublePos2D::operator-(const DoublePos2D& other) const { return {x - other.x, z - other.z}; }


DoublePos2D DoublePos2D::operator-(const double other) const { return {x - other, z - other}; }


bool DoublePos2D::operator>(double value) const { return x > value && z > value; }


bool DoublePos2D::operator<(double value) const { return x < value && z < value; }


bool DoublePos2D::operator>=(double value) const { return x >= value && z >= value; }


bool DoublePos2D::operator<=(double value) const { return x <= value && z <= value; }


std::string DoublePos2D::toString() const { return "(" + std::to_string(x) + ", " + std::to_string(z) + ")"; }


bool DoublePos2D::insideBounds(double lower, double upper) const { return *this > lower && *this < upper; }
