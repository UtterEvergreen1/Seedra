inline const Rotation Rotation::NONE = Rotation(Type::NONE);
inline const Rotation Rotation::CLOCKWISE_90 = Rotation(Type::CLOCKWISE_90);
inline const Rotation Rotation::CLOCKWISE_180 = Rotation(Type::CLOCKWISE_180);
inline const Rotation Rotation::COUNTERCLOCKWISE_90 = Rotation(Type::COUNTERCLOCKWISE_90);

inline const Mirror Mirror::NONE = Mirror(Type::NONE);
inline const Mirror Mirror::LEFT_RIGHT = Mirror(Type::LEFT_RIGHT);
inline const Mirror Mirror::FRONT_BACK = Mirror(Type::FRONT_BACK);

inline Rotation::Rotation(const Type typeIn) : type(typeIn) {
}

inline Rotation Rotation::add(const Rotation &rotation) const {
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

inline bool Rotation::operator==(const Rotation &other) const {
    return this->type == other.type;
}

inline EnumFacing Rotation::apply(const Mirror &mirror, const EnumFacing facing) const {
    EnumFacing mirroredFacing = mirror.mirrorFACING(facing); // Apply mirror first
    switch (type) {
        case Rotation::Type::CLOCKWISE_180:
            return getOpposite(mirroredFacing);
        case Rotation::Type::COUNTERCLOCKWISE_90:
            return rotateYCCW(mirroredFacing);
        case Rotation::Type::CLOCKWISE_90:
            return rotateY(mirroredFacing);
        case Rotation::Type::NONE:
        default:
            return mirroredFacing;
    }
}

inline RotationEnum Rotation::toRotationEnum(const EnumFacing facing) const {
    const EnumAxis axis = getAxis(facing);
    const bool isLeftRightZ = (this->type == Type::CLOCKWISE_180) && (axis == EnumAxis::Y);
    const bool isFrontBackX = (this->type == Type::CLOCKWISE_90) && (axis == EnumAxis::X);

    if (isLeftRightZ || isFrontBackX) {
        return RotationEnum::CLOCKWISE_180;
    }
    return RotationEnum::NONE;
}

inline EnumFacing Rotation::rotateFacing(const EnumFacing facing) const {
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

inline int Rotation::rotate(const int index, const int rotationCount) const {
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

inline std::array<const Rotation *, 4> Rotation::values() {
    return {&NONE, &CLOCKWISE_90, &CLOCKWISE_180, &COUNTERCLOCKWISE_90};
}

inline EnumFacing Rotation::rotateY(const EnumFacing facing) {
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

inline EnumFacing Rotation::rotateYCCW(const EnumFacing facing) {
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

inline EnumFacing Rotation::getOpposite(const EnumFacing facing) {
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

inline Mirror::Mirror(const Type typeIn) : type(typeIn) {
}

inline int Mirror::mirrorRotation(const int rotationIn, const int rotationCount) const {
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

inline Rotation Mirror::toRotation(const EnumFacing facing) const {
    const EnumAxis axis = getAxis(facing);
    const bool isLeftRightZ = (type == Type::LEFT_RIGHT) && (axis == EnumAxis::Z);
    const bool isFrontBackX = (type == Type::FRONT_BACK) && (axis == EnumAxis::X);

    if (isLeftRightZ || isFrontBackX) {
        return Rotation::CLOCKWISE_180;
    }
    return Rotation::NONE;
}

inline EnumFacing Mirror::mirrorFACING(EnumFacing facing) const {
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

inline bool Mirror::operator==(const Mirror &other) const {
    return this->type == other.type;
}

inline std::array<Mirror, 3> Mirror::values() {
    return {NONE, LEFT_RIGHT, FRONT_BACK};
}

