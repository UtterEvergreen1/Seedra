#pragma once

#include "enums.hpp"

#include <array>

enum class RotationEnum {
    NONE,
    CLOCKWISE_90,
    CLOCKWISE_180,
    COUNTERCLOCKWISE_90
};

class Mirror;
class Rotation {
public:
    enum class Type {
        NONE,
        CLOCKWISE_90,
        CLOCKWISE_180,
        COUNTERCLOCKWISE_90
    };

    Type type;

    static const Rotation NONE;
    static const Rotation CLOCKWISE_90;
    static const Rotation CLOCKWISE_180;
    static const Rotation COUNTERCLOCKWISE_90;

    explicit Rotation(Type typeIn);

    [[nodiscard]] Rotation add(const Rotation& rotation) const;

    bool operator==(const Rotation & other) const;

    [[nodiscard]] EnumFacing apply(const Mirror& mirror, EnumFacing facing) const;

    [[nodiscard]] RotationEnum toRotationEnum(EnumFacing facing) const;

    [[nodiscard]] EnumFacing rotateFacing(EnumFacing facing) const;

    [[nodiscard]] int rotate(int index, int rotationCount) const;

    static std::array<const Rotation*, 4> values();

private:
    static EnumFacing rotateY(EnumFacing facing);

    static EnumFacing rotateYCCW(EnumFacing facing);

    static EnumFacing getOpposite(EnumFacing facing);
};


class Mirror {
public:
    enum class Type {
        NONE,
        LEFT_RIGHT,
        FRONT_BACK
    };

    Type type;

    static const Mirror NONE;
    static const Mirror LEFT_RIGHT;
    static const Mirror FRONT_BACK;

    explicit Mirror(Type typeIn);

    [[nodiscard]] int mirrorRotation(int rotationIn, int rotationCount) const;

    [[nodiscard]] Rotation toRotation(EnumFacing facing) const;

    [[nodiscard]] EnumFacing mirrorFACING(EnumFacing facing) const;

    bool operator==(const Mirror & other) const;

    static std::array<Mirror, 3> values();
};

#include "RotationAndMirror.inl"