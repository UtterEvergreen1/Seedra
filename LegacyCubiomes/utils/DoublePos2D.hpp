#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#ifdef INCLUDE_QT
#include <QDebug>
#endif

#include "LegacyCubiomes/cubiomes/processor.hpp"

class DoublePos2D {
public:
    double x;
    double z;

    DoublePos2D() : x(0.0), z(0.0) {}

    DoublePos2D(double xIn, double zIn) : x(xIn), z(zIn) {}

    bool operator==(const DoublePos2D &other) const;
    bool operator==(double other) const;
    bool operator!=(const DoublePos2D &other) const;
    bool operator!=(double other) const;
    DoublePos2D operator+(const DoublePos2D &other) const;
    DoublePos2D operator+(double other) const;
    DoublePos2D operator-(const DoublePos2D &other) const;
    DoublePos2D operator-(double other) const;
    bool operator>(double value) const;
    bool operator<(double value) const;
    bool operator>=(double value) const;
    bool operator<=(double value) const;


    friend std::ostream &operator<<(std::ostream &out, const DoublePos2D &pos) {
        out << "(" << pos.x << ", " << pos.z << ")";
        return out;
    }

#ifdef INCLUDE_QT
    friend QDebug operator<<(QDebug out, const Pos2D &pos) {
        out.nospace() << "(" << pos.x << ", " << pos.z << ")";
        return out.space();
    }
#endif

    MU friend DoublePos2D abs(const DoublePos2D &pos) {
        return {std::abs(pos.x), std::abs(pos.z)};
    }

    MU void setPos(double xIn, double zIn){
        this->x = xIn;
        this->z = zIn;
    }

    MU ND std::string toString() const;
    MU ND bool insideBounds(double lower, double upper) const;

};

typedef std::vector<DoublePos2D> DoublePos2DVec_t;
