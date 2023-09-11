#pragma once

#include <cmath>
#include <iostream>
#include <string>
//#include <QDebug>

class Pos2D {
public:
    int x;
    int z;

    Pos2D() : x(0), z(0) {}

    Pos2D(int x_val, int z_val) : x(x_val), z(z_val) {}

    bool operator==(const Pos2D &other) const;

    bool operator==(int other) const;

    bool operator!=(const Pos2D &other) const;

    bool operator!=(int other) const;

    Pos2D operator+(const Pos2D &other) const;

    Pos2D operator+(int other) const;

    Pos2D operator-(const Pos2D &other) const;

    Pos2D operator-(int other) const;

    bool operator>(int value) const;

    bool operator<(int value) const;

    bool operator>=(int value) const;

    bool operator<=(int value) const;

    Pos2D operator>>(int shiftAmount) const;

    Pos2D operator<<(int shiftAmount) const;

    Pos2D toChunkPos(){
        return {x >> 4, z >> 4};
    }

    Pos2D toBlockPos(){
        return {x << 4, z << 4};
    }

    friend std::ostream &operator<<(std::ostream &out, const Pos2D &pos) {
        out << "(" << pos.x << ", " << pos.z << ")";
        return out;
    }

    /*
    friend QDebug operator<<(QDebug out, const Pos2D &pos) {
        out.nospace() << "(" << pos.x << ", " << pos.z << ")";
        return out.space();
    }
    */

    friend Pos2D abs(const Pos2D &pos) {
        return {std::abs(pos.x), std::abs(pos.z)};
    }

    void setPos(int x, int z){
        this->x = x;
        this->z = z;
    }

    std::string toString();
    bool insideBounds(int lower, int upper);

    struct Hasher {
        std::size_t operator()(const Pos2D &pos) const {
            int i = 1664525 * pos.x + 1013904223;
            int j = 1664525 * (pos.z ^ -559038737) + 1013904223;
            return (size_t) i ^ j;
        }
    };
};
