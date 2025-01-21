#pragma once

#include "enums.hpp"

#include <array>
#include <string>
#include <utility>


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

    explicit Rotation(const Type typeIn) : type(typeIn) {}

    [[nodiscard]] Rotation add(const Rotation& rotation) const {
        switch (rotation.type) {
            case Type::CLOCKWISE_180:
                switch (this->type) {
                    case Type::NONE:
                        return CLOCKWISE_180;
                    case Type::CLOCKWISE_90:
                        return COUNTERCLOCKWISE_90;
                    case Type::CLOCKWISE_180:
                        return NONE;
                    case Type::COUNTERCLOCKWISE_90:
                        return CLOCKWISE_90;
                    default:
                        return *this;
                }

            case Type::COUNTERCLOCKWISE_90:
                switch (this->type) {
                    case Type::NONE:
                        return COUNTERCLOCKWISE_90;
                    case Type::CLOCKWISE_90:
                        return NONE;
                    case Type::CLOCKWISE_180:
                        return CLOCKWISE_90;
                    case Type::COUNTERCLOCKWISE_90:
                        return CLOCKWISE_180;
                    default:
                        return *this;
                }

            case Type::CLOCKWISE_90:
                switch (this->type) {
                    case Type::NONE:
                        return CLOCKWISE_90;
                    case Type::CLOCKWISE_90:
                        return CLOCKWISE_180;
                    case Type::CLOCKWISE_180:
                        return COUNTERCLOCKWISE_90;
                    case Type::COUNTERCLOCKWISE_90:
                        return NONE;
                    default:
                        return *this;
                }

            default:
                return *this;
        }
    }

    bool operator==(const Rotation & other) const {
        return this->type == other.type;
    }

    [[nodiscard]] EnumFacing apply(const Mirror& mirror, EnumFacing facing) const;

    [[nodiscard]] RotationEnum toRotationEnum(const EnumFacing facing) const {
        const EnumAxis axis = getAxis(facing);
        const bool isLeftRightZ = (this->type == Type::CLOCKWISE_180) && (axis == EnumAxis::Y);
        const bool isFrontBackX = (this->type == Type::CLOCKWISE_90) && (axis == EnumAxis::X);

        if (isLeftRightZ || isFrontBackX) {
            return RotationEnum::CLOCKWISE_180;
        }
        return RotationEnum::NONE;
    }

    [[nodiscard]] EnumFacing rotateFacing(const EnumFacing facing) const {
        if (getAxis(facing) == EnumAxis::Y) {
            return facing;
        } else {
            switch (this->type) {
                case Type::CLOCKWISE_90:
                    return rotateY(facing);
                case Type::CLOCKWISE_180:
                    return getOpposite(facing);
                case Type::COUNTERCLOCKWISE_90:
                    return rotateYCCW(facing);
                default:
                    return facing;
            }
        }
    }

    [[nodiscard]] int rotate(const int index, const int rotationCount) const {
        switch (this->type) {
            case Type::CLOCKWISE_90:
                return (index + rotationCount / 4) % rotationCount;
            case Type::CLOCKWISE_180:
                return (index + rotationCount / 2) % rotationCount;
            case Type::COUNTERCLOCKWISE_90:
                return (index + (3 * rotationCount) / 4) % rotationCount;
            default:
                return index;
        }
    }

    static std::array<const Rotation*, 4> values() {
        return { &NONE, &CLOCKWISE_90, &CLOCKWISE_180, &COUNTERCLOCKWISE_90 };
    }

private:
    static EnumFacing rotateY(const EnumFacing facing) {
        switch (facing) {
            case EnumFacing::NORTH:
                return EnumFacing::EAST;
            case EnumFacing::EAST:
                return EnumFacing::SOUTH;
            case EnumFacing::SOUTH:
                return EnumFacing::WEST;
            case EnumFacing::WEST:
                return EnumFacing::NORTH;
            default:
                return facing;
        }
    }

    static EnumFacing rotateYCCW(const EnumFacing facing) {
        switch (facing) {
            case EnumFacing::NORTH:
                return EnumFacing::WEST;
            case EnumFacing::WEST:
                return EnumFacing::SOUTH;
            case EnumFacing::SOUTH:
                return EnumFacing::EAST;
            case EnumFacing::EAST:
                return EnumFacing::NORTH;
            default:
                return facing;
        }
    }

    static EnumFacing getOpposite(const EnumFacing facing) {
        switch (facing) {
            case EnumFacing::NORTH:
                return EnumFacing::SOUTH;
            case EnumFacing::SOUTH:
                return EnumFacing::NORTH;
            case EnumFacing::EAST:
                return EnumFacing::WEST;
            case EnumFacing::WEST:
                return EnumFacing::EAST;
            case EnumFacing::UP:
                return EnumFacing::DOWN;
            case EnumFacing::DOWN:
                return EnumFacing::UP;
            default:
                return facing;
        }
    }
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

    explicit Mirror(const Type typeIn) : type(typeIn) {}

    [[nodiscard]] int mirrorRotation(const int rotationIn, const int rotationCount) const {
        const int halfRotation = rotationCount / 2;
        const int adjustedRotation = rotationIn > halfRotation ? rotationIn - rotationCount : rotationIn;

        switch (type) {
            case Type::FRONT_BACK:
                return (rotationCount - adjustedRotation) % rotationCount;
            case Type::LEFT_RIGHT:
                return (halfRotation - adjustedRotation + rotationCount) % rotationCount;
            case Type::NONE:
            default:
                return rotationIn;
        }
    }

    [[nodiscard]] Rotation toRotation(const EnumFacing facing) const {
        const EnumAxis axis = getAxis(facing);
        const bool isLeftRightZ = (type == Type::LEFT_RIGHT) && (axis == EnumAxis::Z);
        const bool isFrontBackX = (type == Type::FRONT_BACK) && (axis == EnumAxis::X);

        if (isLeftRightZ || isFrontBackX) {
            return Rotation::CLOCKWISE_180;
        }
        return Rotation::NONE;
    }

    [[nodiscard]] EnumFacing mirrorFACING(EnumFacing facing) const {
        switch (type) {
            case Type::FRONT_BACK:
                if (facing == EnumFacing::WEST)
                    return EnumFacing::EAST;
                else if (facing == EnumFacing::EAST)
                    return EnumFacing::WEST;
                break;

            case Type::LEFT_RIGHT:
                if (facing == EnumFacing::NORTH)
                    return EnumFacing::SOUTH;
                else if (facing == EnumFacing::SOUTH)
                    return EnumFacing::NORTH;
                break;

            case Type::NONE:
            default:
                break;
        }
        return facing;
    }

    bool operator==(const Mirror & other) const {
        return this->type == other.type;
    }

    static std::array<Mirror, 3> values() {
        return { NONE, LEFT_RIGHT, FRONT_BACK };
    }
};

