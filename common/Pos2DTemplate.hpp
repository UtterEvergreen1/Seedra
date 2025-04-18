#pragma once


#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#ifdef INCLUDE_QT
#include <QDebug>
#endif

#include "lce/processor.hpp"

template<class T>
class Pos2DTemplate {
public:
    T x;
    T z;

    Pos2DTemplate() : x(0), z(0) {}

    Pos2DTemplate(T xIn, T zIn) : x(xIn), z(zIn) {}

    bool operator==(const Pos2DTemplate& other) const;
    bool operator==(int other) const;
    bool operator!=(const Pos2DTemplate& other) const;
    bool operator!=(int other) const;
    Pos2DTemplate operator+(const Pos2DTemplate& other) const;
    Pos2DTemplate operator+(int other) const;
    Pos2DTemplate operator*(const Pos2DTemplate& other) const;
    Pos2DTemplate operator*(int other) const;
    Pos2DTemplate operator-(const Pos2DTemplate& other) const;
    Pos2DTemplate operator-(int other) const;
    bool operator>(T value) const;
    bool operator<(T value) const;
    bool operator<(const Pos2DTemplate& other) const;
    bool operator>=(T value) const;
    bool operator<=(T value) const;

    template <typename U = T, typename = std::enable_if_t<std::is_integral_v<U>>>
    Pos2DTemplate operator>>(int shiftAmount) const {
        return {x >> shiftAmount, z >> shiftAmount};
    }

    template <typename U = T, typename = std::enable_if_t<std::is_integral_v<U>>>
    Pos2DTemplate operator<<(int shiftAmount) const {
        return {x << shiftAmount, z << shiftAmount};
    }

    template <typename U = T, typename = std::enable_if_t<std::is_integral_v<U>>>
    MU ND Pos2DTemplate toChunkPos() const {
        return {x >> 4, z >> 4};
    }

    template <typename U = T, typename = std::enable_if_t<std::is_integral_v<U>>>
    MU ND Pos2DTemplate toBlockPos() const {
        return {x << 4, z << 4};
    }

    template <typename U, typename = std::enable_if_t<std::is_fundamental_v<U>>>
    MU ND Pos2DTemplate<U> asType() const {
        return { static_cast<U>(x), static_cast<U>(z) };
    }

#ifdef INCLUDE_QT
    friend QDebug operator<<(QDebug out, const Pos2DTemplate<classType>& pos) {
        out.nospace() << "(" << pos.x << ", " << pos.z << ")";
        return out.space();
    }
#endif

    friend std::ostream& operator<<(std::ostream& out, const Pos2DTemplate& pos) {
        out << "(" << pos.x << ", " << pos.z << ")";
        return out;
    }

    MU friend Pos2DTemplate abs(const Pos2DTemplate& pos) {
        return {std::abs(pos.x), std::abs(pos.z)};
    }

    ND double distanceSq() const;

    MU void setPos(T xIn, T zIn);
    MU ND std::string toString() const;
    MU ND bool insideBounds(T lowerX, T lowerZ, T upperX, T upperZ) const;
    MU ND uint64_t asLong() const {
        return (static_cast<uint64_t>(x) << 32) | static_cast<uint64_t>(z);
    }
    MU ND uint64_t hash() const {
        return std::hash<uint64_t>()(asLong());
    }

    struct Hasher {
        template <typename U = T, typename = std::enable_if_t<std::is_integral_v<U>>>
        std::size_t operator()(const Pos2DTemplate& pos) const {
            return pos.hash();
        }
    };
};

typedef Pos2DTemplate<int> Pos2D;
typedef std::vector<Pos2D> Pos2DVec_t;

typedef Pos2DTemplate<double> DoublePos2D;

#include "Pos2DTemplate.inl"