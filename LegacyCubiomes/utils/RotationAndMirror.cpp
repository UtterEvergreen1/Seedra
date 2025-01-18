#include "RotationAndMirror.hpp"

const Rotation Rotation::NONE = Rotation(Type::NONE);
const Rotation Rotation::CLOCKWISE_90 = Rotation(Type::CLOCKWISE_90);
const Rotation Rotation::CLOCKWISE_180 = Rotation(Type::CLOCKWISE_180);
const Rotation Rotation::COUNTERCLOCKWISE_90 = Rotation(Type::COUNTERCLOCKWISE_90);

const Mirror Mirror::NONE = Mirror(Type::NONE);
const Mirror Mirror::LEFT_RIGHT = Mirror(Type::LEFT_RIGHT);
const Mirror Mirror::FRONT_BACK = Mirror(Type::FRONT_BACK);

enumFacing Rotation::apply(const Mirror& mirror, const enumFacing facing) const {
    enumFacing mirroredFacing = mirror.mirrorFACING(facing); // Apply mirror first
    switch (type) {
        case Rotation::Type::CLOCKWISE_180:
            return getOpposite(mirroredFacing);
        case Rotation::Type::COUNTERCLOCKWISE_90:
            return rotateYCCW(mirroredFacing);
        case Rotation::Type::CLOCKWISE_90:
            return rotateY(mirroredFacing);
        case Rotation::Type::NONE:
        default:
            return facing;
    }
}
