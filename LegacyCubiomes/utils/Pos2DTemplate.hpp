#pragma once


#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#ifdef INCLUDE_QT
#include <QDebug>
#endif

#include "lce/processor.hpp"


template<class classType>
class Pos2DTemplate {
public:
    classType x;
    classType z;

    Pos2DTemplate() : x(0), z(0) {}

    Pos2DTemplate(classType xIn, classType zIn) : x(xIn), z(zIn) {}

    bool operator==(const Pos2DTemplate& other) const;
    bool operator==(int other) const;
    bool operator!=(const Pos2DTemplate& other) const;
    bool operator!=(int other) const;
    Pos2DTemplate operator+(const Pos2DTemplate& other) const;
    Pos2DTemplate operator+(int other) const;
    Pos2DTemplate operator-(const Pos2DTemplate& other) const;
    Pos2DTemplate operator-(int other) const;
    bool operator>(classType value) const;
    bool operator<(classType value) const;
    bool operator>=(classType value) const;
    bool operator<=(classType value) const;

    template <typename T = classType, typename = std::enable_if_t<std::is_integral_v<T>>>
    Pos2DTemplate operator>>(int shiftAmount) const {
        return {x >> shiftAmount, z >> shiftAmount};
    }

    template <typename T = classType, typename = std::enable_if_t<std::is_integral_v<T>>>
    Pos2DTemplate operator<<(int shiftAmount) const {
        return {x << shiftAmount, z << shiftAmount};
    }

    template <typename T = classType, typename = std::enable_if_t<std::is_integral_v<T>>>
    MU ND Pos2DTemplate toChunkPos() const {
        return {x >> 4, z >> 4};
    }

    template <typename T = classType, typename = std::enable_if_t<std::is_integral_v<T>>>
    MU ND Pos2DTemplate toBlockPos() const {
        return {x << 4, z << 4};
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

    MU void setPos(classType xIn, classType zIn);
    MU ND std::string toString() const;
    MU ND bool insideBounds(classType lowerX, classType lowerZ, classType upperX, classType upperZ) const;

    struct Hasher {
        template <typename T = classType, typename = std::enable_if_t<std::is_integral_v<T>>>
        std::size_t operator()(const Pos2DTemplate& pos) const {
            c_int i = 1664525 * pos.x + 1013904223;
            c_int j = 1664525 * (pos.z ^ -559038737) + 1013904223;
            return static_cast<size_t>(i) ^ j;
        }
    };
};

typedef Pos2DTemplate<int> Pos2D;
typedef std::vector<Pos2D> Pos2DVec_t;

typedef Pos2DTemplate<double> DoublePos2D;