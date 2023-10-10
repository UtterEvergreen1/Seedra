#pragma once

#include <cmath>
#include <ostream>
#ifdef INCLUDE_QT
#include <QDebug>
#endif


#include "LegacyCubiomes/cubiomes/processor.hpp"
#include "pos2D.hpp"

class Pos3D {
private:
    int x;
    int y;
    int z;

public:
    Pos3D() : x(0), y(0), z(0) {}
    explicit Pos3D(Pos2D pos) : x(pos.x), y(0), z(pos.z) {}
    Pos3D(int xIn, int yIn, int zIn) : x(xIn), y(yIn), z(zIn) {}

    inline int getX() {return x;}
    inline int getY() {return y;}
    inline int getZ() {return z;}

    bool  operator==(const Pos3D& other) const;
    Pos3D operator +(const Pos3D& other) const;
    Pos3D operator +(int other) const;
    Pos3D operator -(const Pos3D& other) const;
    Pos3D operator -(int other) const;
    bool  operator >(int value) const;
    bool  operator <(int value) const;
    bool  operator>=(int value) const;
    bool  operator<=(int value) const;
    Pos3D operator>>(int shiftAmount) const;
    Pos3D operator<<(int shiftAmount) const;

    friend std::ostream& operator<<(std::ostream &out, const Pos3D &pos) {
        out << "[" << pos.x << ", " << pos.y << ", " <<pos.z << "]";
        return out;
    }

#ifdef INCLUDE_QT
    friend QDebug operator<<(QDebug out, const Pos3D &pos) {
        out.nospace() << "(" << pos.x << ", " << pos.y << ", " << pos.z << ")";
        return out.space();
    }
#endif

    friend Pos3D abs(const Pos3D& pos) {
        return { std::abs(pos.x), std::abs(pos.y), std::abs(pos.z) };
    }

    void setPos(int xIn, int yIn, int zIn){
        this->x = xIn;
        this->y = yIn;
        this->z = zIn;
    }


    MU ND const Pos2D convert2D() const;
    MU ND bool insideBounds(int lower, int upper);
};

typedef std::vector<Pos3D> Pos3DVec_t;