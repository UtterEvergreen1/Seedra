#pragma once

#include <cmath>
#include <ostream>

#ifdef INCLUDE_QT

#include <QDebug>

#endif


#include "lce/processor.hpp"
#include "lce/enums.hpp"
#include "Pos2DTemplate.hpp"

template<typename T>
class Pos3DTemplate {
public:
    T x, y, z;

    Pos3DTemplate() : x(0), y(0), z(0) {}

    explicit Pos3DTemplate(Pos2DTemplate<T> pos) : x(pos.x), y(0), z(pos.z) {}

    Pos3DTemplate(T xIn, T yIn, T zIn) : x(xIn), y(yIn), z(zIn) {}


    bool operator==(const Pos3DTemplate &other) const;
    void operator+=(const Pos2DTemplate<T> &other);
    void operator-=(const Pos2DTemplate<T> &other);

    Pos3DTemplate operator+(const Pos3DTemplate &other) const;
    Pos3DTemplate operator+(const Pos2DTemplate<T> &other) const;
    Pos3DTemplate operator+(T other) const;

    Pos3DTemplate operator-(const Pos3DTemplate &other) const;
    Pos3DTemplate operator-(const Pos2DTemplate<T> &other) const;
    Pos3DTemplate operator-(T other) const;


    bool operator>(int value) const;
    bool operator<(int value) const;
    bool operator>=(int value) const;
    bool operator<=(int value) const;

    template<typename U = T, typename = std::enable_if_t<std::is_integral_v<U>>>
    Pos3DTemplate operator>>(int shiftAmount) const {
        return {x >> shiftAmount, y >> shiftAmount, z >> shiftAmount};
    }

    template<typename U = T, typename = std::enable_if_t<std::is_integral_v<U>>>
    Pos3DTemplate operator<<(int shiftAmount) const {
        return {x << shiftAmount, y << shiftAmount, z << shiftAmount};
    }

    template <typename U, typename = std::enable_if_t<std::is_fundamental_v<U>>>
    MU ND Pos3DTemplate<U> asType() const {
        Pos3DTemplate<U> pos(static_cast<U>(x), static_cast<U>(y), static_cast<U>(z));
        return pos;
    }

    template<typename V = T, typename = std::enable_if_t<std::is_integral_v<V>>>
    MU ND Pos3DTemplate convertToChunkCoords() const;

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

    MU ND T getX() const { return x; }
    MU ND T getY() const { return y; }
    MU ND T getZ() const { return z; }

    MU ND bool isNull() const { return x == 0 && y == 0 && z == 0; }

    friend Pos3DTemplate abs(const Pos3DTemplate &pos) {
        return {std::abs(pos.x), std::abs(pos.y), std::abs(pos.z)};
    }

    void setPos(T xIn, T yIn, T zIn) {
        this->x = xIn;
        this->y = yIn;
        this->z = zIn;
    }

    Pos3DTemplate add(T xOff, T yOff, T zOff) const {
        return {x + xOff, y + yOff, z + zOff};
    }

    /// minus yOff
    Pos3DTemplate down(T yOff = 1) const { return {x, y - yOff, z}; }

    /// plus yOff
    Pos3DTemplate up(T yOff = 1) const { return {x, y + yOff, z}; }

    /// plus xOff
    Pos3DTemplate east(T xOff = 1) const { return {x + xOff, y, z}; }

    /// minus xOff
    Pos3DTemplate west(T xOff = 1) const { return {x - xOff, y, z}; }

    /// minus zOff
    Pos3DTemplate north(T zOff = 1) const { return {x, y, z - zOff}; }

    /// plus zOff
    Pos3DTemplate south(T zOff = 1) const { return {x, y, z + zOff}; }

    Pos3DTemplate offset(EnumFacing facing, int n = 1) const;

    ND double distanceSqXZ() const;
    ND double distanceSq() const;
    ND double distanceSq(double toX, double toY, double toZ) const;
    ND double distanceSq(const Pos3DTemplate &to) const {
        return distanceSq(static_cast<double>(to.x), static_cast<double>(to.y), static_cast<double>(to.z));
    }

    MU ND Pos2DTemplate<T> asPos2D() const;

    static std::vector<Pos3DTemplate> getAllInBox(const Pos3DTemplate& from, const Pos3DTemplate& to);
};


typedef Pos3DTemplate<int> Pos3D;
typedef std::vector<Pos3D> Pos3DVec_t;

typedef Pos3DTemplate<double> DoublePos3D;