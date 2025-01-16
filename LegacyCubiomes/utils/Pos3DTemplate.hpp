#pragma once

#include <cmath>
#include <ostream>

#ifdef INCLUDE_QT

#include <QDebug>

#endif


#include "enums.hpp"
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

    MU ND classType getX() const { return x; }

    MU ND classType getY() const { return y; }

    MU ND classType getZ() const { return z; }

    MU ND bool isNull() const { return x == 0 && y == 0 && z == 0; }

    bool operator==(const Pos3DTemplate &other) const;

    Pos3DTemplate operator+(const Pos3DTemplate &other) const;

    Pos3DTemplate operator+(classType other) const;

    Pos3DTemplate operator-(const Pos3DTemplate &other) const;

    Pos3DTemplate operator-(classType other) const;

    bool operator>(int value) const;

    bool operator<(int value) const;

    bool operator>=(int value) const;

    bool operator<=(int value) const;

    template<typename T = classType, typename = std::enable_if_t<std::is_integral_v<T>>>
    Pos3DTemplate operator>>(int shiftAmount) const {
        return {x >> shiftAmount, y >> shiftAmount, z >> shiftAmount};
    }

    template<typename T = classType, typename = std::enable_if_t<std::is_integral_v<T>>>
    Pos3DTemplate operator<<(int shiftAmount) const {
        return {x << shiftAmount, y << shiftAmount, z << shiftAmount};
    }

#ifdef INCLUDE_QT

    friend QDebug operator<<(QDebug out, const Pos3DTemplate<classType> &pos) {
        out.nospace() << "(" << pos.x << ", " << pos.y << ", " << pos.z << ")";
        return out.space();
    }

#endif

    friend std::ostream &operator<<(std::ostream &out, const Pos3DTemplate &pos) {
        out << "[" << pos.x << ", " << pos.y << ", " << pos.z << "]";
        return out;
    }

    friend Pos3DTemplate abs(const Pos3DTemplate &pos) {
        return {std::abs(pos.x), std::abs(pos.y), std::abs(pos.z)};
    }

    void setPos(classType xIn, classType yIn, classType zIn) {
        this->x = xIn;
        this->y = yIn;
        this->z = zIn;
    }

    Pos3DTemplate add(classType xOff, classType yOff, classType zOff) const {
        return {x + xOff, y + yOff, z + zOff};
    }

    Pos3DTemplate down(classType yOff = 1) const {
        return {x, y - yOff, z};
    }

    Pos3DTemplate up(classType yOff = 1) const {
        return {x, y + yOff, z};
    }

    Pos3DTemplate east(classType xOff = 1) const {
        return {x + xOff, y, z};
    }

    Pos3DTemplate west(classType xOff = 1) const {
        return {x - xOff, y, z};
    }

    Pos3DTemplate north(classType zOff = 1) const {
        return {x, y, z - zOff};
    }

    Pos3DTemplate south(classType zOff = 1) const {
        return {x, y, z + zOff};
    }

    Pos3DTemplate offset(enumFacing facing, int n = 1) const;

    ND double distanceSq(c_double toX, c_double toY, c_double toZ) const;
    ND double distanceSq(const Pos3DTemplate &to) const {
        return distanceSq(static_cast<double>(to.x), static_cast<double>(to.y), static_cast<double>(to.z));
    }

    MU ND Pos2DTemplate<classType> convert2D() const;

    // only works for integral types
    template<typename T = classType, typename = std::enable_if_t<std::is_integral_v<T>>> MU ND
    Pos3DTemplate convertToChunkCords() const;

    MU ND bool insideBounds(classType lowerX, classType lowerY, classType lowerZ, classType upperX, classType upperY,
                            classType upperZ) const;

    static std::vector<Pos3DTemplate> getAllInBox(const Pos3DTemplate& from, const Pos3DTemplate& to);
};

typedef Pos3DTemplate<int> Pos3D;
typedef std::vector<Pos3D> Pos3DVec_t;

typedef Pos3DTemplate<double> DoublePos3D;