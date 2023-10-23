#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#ifdef INCLUDE_QT
#include <QDebug>
#endif

#include "processor.hpp"

class Pos2D {
public:
    int x;
    int z;

    Pos2D() : x(0), z(0) {}

    Pos2D(int xIn, int zIn) : x(xIn), z(zIn) {}

    bool operator==(const Pos2D& other) const;
    bool operator==(int other) const;
    bool operator!=(const Pos2D& other) const;
    bool operator!=(int other) const;
    Pos2D operator+(const Pos2D& other) const;
    Pos2D operator+(int other) const;
    Pos2D operator-(const Pos2D& other) const;
    Pos2D operator-(int other) const;
    bool operator>(int value) const;
    bool operator<(int value) const;
    bool operator>=(int value) const;
    bool operator<=(int value) const;
    Pos2D operator>>(int shiftAmount) const;
    Pos2D operator<<(int shiftAmount) const;

    MU ND Pos2D toChunkPos() const { return {x >> 4, z >> 4}; }

    MU ND Pos2D toBlockPos() const { return {x << 4, z << 4}; }

    friend std::ostream& operator<<(std::ostream& out, const Pos2D& pos) {
        out << "(" << pos.x << ", " << pos.z << ")";
        return out;
    }

#ifdef INCLUDE_QT
    friend QDebug operator<<(QDebug out, const Pos2D& pos) {
        out.nospace() << "(" << pos.x << ", " << pos.z << ")";
        return out.space();
    }
#endif

    MU friend Pos2D abs(const Pos2D& pos) { return {std::abs(pos.x), std::abs(pos.z)}; }

    MU void setPos(int xIn, int zIn) {
        this->x = xIn;
        this->z = zIn;
    }

    MU ND std::string toString();
    MU ND bool insideBounds(int lower, int upper);

    struct Hasher {
        std::size_t operator()(const Pos2D& pos) const {
            int i = 1664525 * pos.x + 1013904223;
            int j = 1664525 * (pos.z ^ -559038737) + 1013904223;
            return (size_t) i ^ j;
        }
    };
};

typedef std::vector<Pos2D> Pos2DVec_t;
