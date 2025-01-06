#pragma once

#include "enums.hpp"
#include <array>
#include <string>


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

    Rotation(Type typeIn, const std::string& nameIn) : type(typeIn), name(nameIn) {}

    Rotation add(const Rotation& rotation) const {
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

    RotationEnum toRotationEnum(FACING facing) const {
        EnumAxis axis = getAxis(facing);
        bool isLeftRightZ = (this->type == Type::CLOCKWISE_180) && (axis == EnumAxis::Y);
        bool isFrontBackX = (this->type == Type::CLOCKWISE_90) && (axis == EnumAxis::X);

        if (isLeftRightZ || isFrontBackX) {
            return RotationEnum::CLOCKWISE_180;
        }
        return RotationEnum::NONE;
    }

    FACING rotateFacing(FACING facing) const {
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

    int rotate(int index, int rotationCount) const {
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

    const std::string& getName() const {
        return name;
    }

    static std::array<const Rotation*, 4> values() {
        return { &NONE, &CLOCKWISE_90, &CLOCKWISE_180, &COUNTERCLOCKWISE_90 };
    }

private:
    FACING rotateY(FACING facing) const {
        switch (facing) {
            case FACING::NORTH:
                return FACING::EAST;
            case FACING::EAST:
                return FACING::SOUTH;
            case FACING::SOUTH:
                return FACING::WEST;
            case FACING::WEST:
                return FACING::NORTH;
            default:
                return facing;
        }
    }

    FACING rotateYCCW(FACING facing) const {
        switch (facing) {
            case FACING::NORTH:
                return FACING::WEST;
            case FACING::WEST:
                return FACING::SOUTH;
            case FACING::SOUTH:
                return FACING::EAST;
            case FACING::EAST:
                return FACING::NORTH;
            default:
                return facing;
        }
    }

    FACING getOpposite(FACING facing) const {
        switch (facing) {
            case FACING::NORTH:
                return FACING::SOUTH;
            case FACING::SOUTH:
                return FACING::NORTH;
            case FACING::EAST:
                return FACING::WEST;
            case FACING::WEST:
                return FACING::EAST;
            case FACING::UP:
                return FACING::DOWN;
            case FACING::DOWN:
                return FACING::UP;
            default:
                return facing;
        }
    }
};

const std::array<std::string, 4> Rotation::rotationNames = { "rotate_0", "rotate_90", "rotate_180", "rotate_270" };

const Rotation Rotation::NONE(Rotation::Type::NONE, "rotate_0");
const Rotation Rotation::CLOCKWISE_90(Rotation::Type::CLOCKWISE_90, "rotate_90");
const Rotation Rotation::CLOCKWISE_180(Rotation::Type::CLOCKWISE_180, "rotate_180");
const Rotation Rotation::COUNTERCLOCKWISE_90(Rotation::Type::COUNTERCLOCKWISE_90, "rotate_270");


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

    Mirror(Type typeIn, std::string  nameIn) : type(typeIn), name(std::move(nameIn)) {}

    int mirrorRotation(int rotationIn, int rotationCount) const {
        int halfRotation = rotationCount / 2;
        int adjustedRotation = rotationIn > halfRotation ? rotationIn - rotationCount : rotationIn;

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

    Rotation toRotation(FACING facing) const {
        EnumAxis axis = getAxis(facing);
        bool isLeftRightZ = (type == Type::LEFT_RIGHT) && (axis == EnumAxis::Z);
        bool isFrontBackX = (type == Type::FRONT_BACK) && (axis == EnumAxis::X);

        if (isLeftRightZ || isFrontBackX) {
            return Rotation::CLOCKWISE_180;
        }
        return Rotation::NONE;
    }

    FACING mirrorFACING(FACING facing) const {
        switch (type) {
            case Type::FRONT_BACK:
                if (facing == FACING::WEST)
                    return FACING::EAST;
                else if (facing == FACING::EAST)
                    return FACING::WEST;
                break;

            case Type::LEFT_RIGHT:
                if (facing == FACING::NORTH)
                    return FACING::SOUTH;
                else if (facing == FACING::SOUTH)
                    return FACING::NORTH;
                break;

            case Type::NONE:
            default:
                break;
        }
        return facing;
    }

    const std::string& getName() const {
        return name;
    }

    static std::array<Mirror, 3> values() {
        return { Mirror::NONE, Mirror::LEFT_RIGHT, Mirror::FRONT_BACK };
    }
};

const std::array<std::string, 3> Mirror::mirrorNames = { "no_mirror", "mirror_left_right", "mirror_front_back" };

const Mirror Mirror::NONE(Mirror::Type::NONE, "no_mirror");
const Mirror Mirror::LEFT_RIGHT(Mirror::Type::LEFT_RIGHT, "mirror_left_right");
const Mirror Mirror::FRONT_BACK(Mirror::Type::FRONT_BACK, "mirror_front_back");

