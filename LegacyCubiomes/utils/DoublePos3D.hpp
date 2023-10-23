#pragma once

#include <cmath>
#include <ostream>
#ifdef INCLUDE_QT
#include <QDebug>
#endif


#include "DoublePos2D.hpp"
#include "processor.hpp"

class DoublePos3D {
public:
    double x;
    double y;
    double z;

    DoublePos3D() : x(0), y(0), z(0) {}
    explicit DoublePos3D(DoublePos2D pos) : x(pos.x), y(0), z(pos.z) {}
    DoublePos3D(double xIn, double yIn, double zIn) : x(xIn), y(yIn), z(zIn) {}

    bool operator==(const DoublePos3D& other) const;
    DoublePos3D operator+(const DoublePos3D& other) const;
    DoublePos3D operator+(double other) const;
    DoublePos3D operator-(const DoublePos3D& other) const;
    DoublePos3D operator-(double other) const;
    bool operator>(double value) const;
    bool operator<(double value) const;
    bool operator>=(double value) const;
    bool operator<=(double value) const;

    friend std::ostream& operator<<(std::ostream& out, const DoublePos3D& pos) {
        out << "[" << pos.x << ", " << pos.y << ", " << pos.z << "]";
        return out;
    }

#ifdef INCLUDE_QT
    friend QDebug operator<<(QDebug out, const DoublePos3D& pos) {
        out.nospace() << "(" << pos.x << ", " << pos.y << ", " << pos.z << ")";
        return out.space();
    }
#endif

    void setPos(int xIn, int yIn, int zIn) {
        this->x = xIn;
        this->y = yIn;
        this->z = zIn;
    }


    MU ND DoublePos2D convert2D();
    MU ND bool insideBounds(double lower, double upper);
};

typedef std::vector<DoublePos3D> DoublePos3DVec_t;