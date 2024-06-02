#pragma once

#include <cmath>
#include <ostream>
#ifdef INCLUDE_QT
#include <QDebug>
#endif


#include "Pos2DTemplate.hpp"
#include "lce/processor.hpp"

template<class classType>
class Pos3DTemplate {
public:
    classType x;
    classType y;
    classType z;

    Pos3DTemplate() : x(0), y(0), z(0) {}
    explicit Pos3DTemplate(Pos2DTemplate<classType> pos) : x(pos.x), y(0), z(pos.z) {}
    Pos3DTemplate(classType xIn, classType yIn, classType zIn) : x(xIn), y(yIn), z(zIn) {}

    MU ND inline classType getX() const { return x; }
    MU ND inline classType getY() const { return y; }
    MU ND inline classType getZ() const { return z; }

    bool operator==(const Pos3DTemplate& other) const;
    Pos3DTemplate operator+(const Pos3DTemplate& other) const;
    Pos3DTemplate operator+(classType other) const;
    Pos3DTemplate operator-(const Pos3DTemplate& other) const;
    Pos3DTemplate operator-(classType other) const;
    bool operator>(int value) const;
    bool operator<(int value) const;
    bool operator>=(int value) const;
    bool operator<=(int value) const;

    template <typename T = classType, typename = std::enable_if_t<std::is_integral_v<T>>>
    Pos3DTemplate operator>>(int shiftAmount) const {
        return {x >> shiftAmount, y >> shiftAmount, z >> shiftAmount};
    }

    template <typename T = classType, typename = std::enable_if_t<std::is_integral_v<T>>>
    Pos3DTemplate operator<<(int shiftAmount) const {
        return {x << shiftAmount, y << shiftAmount, z << shiftAmount};
    }

#ifdef INCLUDE_QT
    friend QDebug operator<<(QDebug out, const Pos3DTemplate<classType>& pos) {
        out.nospace() << "(" << pos.x << ", " << pos.y << ", " << pos.z << ")";
        return out.space();
    }
#endif

    friend std::ostream& operator<<(std::ostream& out, const Pos3DTemplate& pos) {
        out << "[" << pos.x << ", " << pos.y << ", " << pos.z << "]";
        return out;
    }

    friend Pos3DTemplate abs(const Pos3DTemplate& pos) {
        return {std::abs(pos.x), std::abs(pos.y), std::abs(pos.z)};
    }

    void setPos(int xIn, int yIn, int zIn) {
        this->x = xIn;
        this->y = yIn;
        this->z = zIn;
    }

    MU ND Pos2DTemplate<classType> convert2D() const;
    MU ND bool insideBounds(classType lower, classType upper);
};

typedef Pos3DTemplate<int> Pos3D;
typedef std::vector<Pos3D> Pos3DVec_t;

typedef Pos3DTemplate<double> DoublePos3D;