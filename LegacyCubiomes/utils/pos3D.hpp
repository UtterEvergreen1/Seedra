#pragma once


#pragma once

#include <cmath>
#include <ostream>
#include "pos2D.hpp"

class Pos3D
{
public:
    int x;
    int y;
    int z;

    Pos3D() : x(0), y(0), z(0) {}
    Pos3D(int x_val, int y_val, int z_val) : x(x_val), y(y_val), z(z_val) {}

    bool operator==(const Pos3D& other) const;
    Pos3D operator+(const Pos3D& other) const;
    Pos3D operator+(int other) const;
    Pos3D operator-(const Pos3D& other) const;
    Pos3D operator-(int other) const;
    bool operator>(int value) const;
    bool operator<(int value) const;
    bool operator>=(int value) const;
    bool operator<=(int value) const;
    Pos3D operator>>(int shiftAmount) const;
    Pos3D operator<<(int shiftAmount) const;

    friend std::ostream& operator<<(std::ostream &out, const Pos3D &pos) {
        out << "[" << pos.x << ", " << pos.y << ", " <<pos.z << "]";
        return out;
    }

    /*
    friend QDebug operator<<(QDebug out, const Pos3D &pos) {
        out.nospace() << "(" << pos.x << ", " << pos.y << ", " << pos.z << ")";
        return out.space();
    }
     */

    friend Pos3D abs(const Pos3D& pos) {
        return { std::abs(pos.x), std::abs(pos.y), std::abs(pos.z) };
    }

    void setPos(int x, int y, int z){
        this->x = x;
        this->y = y;
        this->z = z;
    }


    Pos2D convert2D();
    bool insideBounds(int lower, int upper);
};
