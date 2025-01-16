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

class Rotation {
public:
    enum class Type {
        NONE,
        CLOCKWISE_90,
        CLOCKWISE_180,
        COUNTERCLOCKWISE_90
    };

private:
    Type type;
    std::string name;
    static const std::array<std::string, 4> rotationNames;

public:
    static const Rotation NONE;
    static const Rotation CLOCKWISE_90;
    static const Rotation CLOCKWISE_180;
    static const Rotation COUNTERCLOCKWISE_90;

    Rotation(const Type typeIn, std::string nameIn) : type(typeIn), name(std::move(nameIn)) {}

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

    [[nodiscard]] RotationEnum toRotationEnum(const enumFacing facing) const {
        const EnumAxis axis = getAxis(facing);
        const bool isLeftRightZ = (this->type == Type::CLOCKWISE_180) && (axis == EnumAxis::Y);
        const bool isFrontBackX = (this->type == Type::CLOCKWISE_90) && (axis == EnumAxis::X);

        if (isLeftRightZ || isFrontBackX) {
            return RotationEnum::CLOCKWISE_180;
        }
        return RotationEnum::NONE;
    }

    [[nodiscard]] enumFacing rotateFacing(const enumFacing facing) const {
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

    [[nodiscard]] const std::string& getName() const {
        return name;
    }

    static std::array<const Rotation*, 4> values() {
        return { &NONE, &CLOCKWISE_90, &CLOCKWISE_180, &COUNTERCLOCKWISE_90 };
    }

private:
    static enumFacing rotateY(const enumFacing facing) {
        switch (facing) {
            case enumFacing::NORTH:
                return enumFacing::EAST;
            case enumFacing::EAST:
                return enumFacing::SOUTH;
            case enumFacing::SOUTH:
                return enumFacing::WEST;
            case enumFacing::WEST:
                return enumFacing::NORTH;
            default:
                return facing;
        }
    }

    static enumFacing rotateYCCW(const enumFacing facing) {
        switch (facing) {
            case enumFacing::NORTH:
                return enumFacing::WEST;
            case enumFacing::WEST:
                return enumFacing::SOUTH;
            case enumFacing::SOUTH:
                return enumFacing::EAST;
            case enumFacing::EAST:
                return enumFacing::NORTH;
            default:
                return facing;
        }
    }

    static enumFacing getOpposite(const enumFacing facing) {
        switch (facing) {
            case enumFacing::NORTH:
                return enumFacing::SOUTH;
            case enumFacing::SOUTH:
                return enumFacing::NORTH;
            case enumFacing::EAST:
                return enumFacing::WEST;
            case enumFacing::WEST:
                return enumFacing::EAST;
            case enumFacing::UP:
                return enumFacing::DOWN;
            case enumFacing::DOWN:
                return enumFacing::UP;
            default:
                return facing;
        }
    }
};

const std::array<std::string, 4> Rotation::rotationNames = { "rotate_0", "rotate_90", "rotate_180", "rotate_270" };

const Rotation Rotation::NONE(Type::NONE, "rotate_0");
const Rotation Rotation::CLOCKWISE_90(Type::CLOCKWISE_90, "rotate_90");
const Rotation Rotation::CLOCKWISE_180(Type::CLOCKWISE_180, "rotate_180");
const Rotation Rotation::COUNTERCLOCKWISE_90(Type::COUNTERCLOCKWISE_90, "rotate_270");


class Mirror {
public:
    enum class Type {
        NONE,
        LEFT_RIGHT,
        FRONT_BACK
    };

private:
    Type type;
    std::string name;
    static const std::array<std::string, 3> mirrorNames;

public:
    static const Mirror NONE;
    static const Mirror LEFT_RIGHT;
    static const Mirror FRONT_BACK;

    Mirror(const Type typeIn, std::string  nameIn) : type(typeIn), name(std::move(nameIn)) {}

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

    [[nodiscard]] Rotation toRotation(const enumFacing facing) const {
        const EnumAxis axis = getAxis(facing);
        const bool isLeftRightZ = (type == Type::LEFT_RIGHT) && (axis == EnumAxis::Z);
        const bool isFrontBackX = (type == Type::FRONT_BACK) && (axis == EnumAxis::X);

        if (isLeftRightZ || isFrontBackX) {
            return Rotation::CLOCKWISE_180;
        }
        return Rotation::NONE;
    }

    [[nodiscard]] enumFacing mirrorFACING(enumFacing facing) const {
        switch (type) {
            case Type::FRONT_BACK:
                if (facing == enumFacing::WEST)
                    return enumFacing::EAST;
                else if (facing == enumFacing::EAST)
                    return enumFacing::WEST;
                break;

            case Type::LEFT_RIGHT:
                if (facing == enumFacing::NORTH)
                    return enumFacing::SOUTH;
                else if (facing == enumFacing::SOUTH)
                    return enumFacing::NORTH;
                break;

            case Type::NONE:
            default:
                break;
        }
        return facing;
    }

    [[nodiscard]] const std::string& getName() const {
        return name;
    }

    static std::array<Mirror, 3> values() {
        return { NONE, LEFT_RIGHT, FRONT_BACK };
    }
};

const std::array<std::string, 3> Mirror::mirrorNames = { "no_mirror", "mirror_left_right", "mirror_front_back" };

const Mirror Mirror::NONE(Type::NONE, "no_mirror");
const Mirror Mirror::LEFT_RIGHT(Type::LEFT_RIGHT, "mirror_left_right");
const Mirror Mirror::FRONT_BACK(Type::FRONT_BACK, "mirror_front_back");

