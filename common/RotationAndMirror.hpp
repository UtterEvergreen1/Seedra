#pragma once

#include "enums.hpp"

#include <array>

/**
 * @enum RotationEnum
 * @brief Represents the possible rotation states.
 */
enum class RotationEnum {
    NONE, ///< No rotation.
    CLOCKWISE_90, ///< 90 degrees clockwise rotation.
    CLOCKWISE_180, ///< 180 degrees clockwise rotation.
    COUNTERCLOCKWISE_90 ///< 90 degrees counterclockwise rotation.
};

class Mirror;

class Rotation {
public:
    /**
     * @enum Type
     * @brief Represents the types of rotations.
     */
    enum class Type {
        NONE, ///< No rotation.
        CLOCKWISE_90, ///< 90 degrees clockwise rotation.
        CLOCKWISE_180, ///< 180 degrees clockwise rotation.
        COUNTERCLOCKWISE_90 ///< 90 degrees counterclockwise rotation.
    };

    Type type; ///< The type of rotation.

    static const Rotation NONE; ///< No rotation.
    static const Rotation CLOCKWISE_90; ///< 90 degrees clockwise rotation.
    static const Rotation CLOCKWISE_180; ///< 180 degrees clockwise rotation.
    static const Rotation COUNTERCLOCKWISE_90; ///< 90 degrees counterclockwise rotation.

    /**
     * @brief Constructs a Rotation object with the specified type.
     * @param typeIn The type of rotation.
     */
    explicit Rotation(Type typeIn);

    /**
     * @brief Adds another rotation to this rotation.
     * @param rotation The rotation to add.
     * @return The resulting rotation.
     */
    [[nodiscard]] Rotation add(const Rotation &rotation) const;

    /**
     * @brief Equality operator to compare two Rotation objects.
     * @param other The other Rotation object.
     * @return True if the rotations are equal, false otherwise.
     */
    bool operator==(const Rotation &other) const;

    /**
     * @brief Applies the rotation to a facing direction based on a mirror.
     * @param mirror The mirror to apply.
     * @param facing The facing direction.
     * @return The resulting facing direction.
     */
    [[nodiscard]] EnumFacing apply(const Mirror &mirror, EnumFacing facing) const;

    /**
     * @brief Converts a facing direction to a RotationEnum.
     * @param facing The facing direction.
     * @return The corresponding RotationEnum.
     */
    [[nodiscard]] RotationEnum toRotationEnum(EnumFacing facing) const;

    /**
     * @brief Rotates a facing direction based on this rotation.
     * @param facing The facing direction.
     * @return The rotated facing direction.
     */
    [[nodiscard]] EnumFacing rotateFacing(EnumFacing facing) const;

    /**
     * @brief Rotates an index by a specified number of rotations.
     * @param index The index to rotate.
     * @param rotationCount The number of rotations to apply.
     * @return The rotated index.
     */
    [[nodiscard]] int rotate(int index, int rotationCount) const;

    /**
     * @brief Gets all possible rotation values.
     * @return An array of pointers to the possible rotations.
     */
    static std::array<const Rotation *, 4> values();

private:
    /**
     * @brief Rotates a facing direction clockwise.
     * @param facing The facing direction.
     * @return The rotated facing direction.
     */
    static EnumFacing rotateY(EnumFacing facing);

    /**
     * @brief Rotates a facing direction counterclockwise.
     * @param facing The facing direction.
     * @return The rotated facing direction.
     */
    static EnumFacing rotateYCCW(EnumFacing facing);

    /**
     * @brief Gets the opposite of a facing direction.
     * @param facing The facing direction.
     * @return The opposite facing direction.
     */
    static EnumFacing getOpposite(EnumFacing facing);
};

class Mirror {
public:
    /**
     * @enum Type
     * @brief Represents the types of mirrors.
     */
    enum class Type {
        NONE, ///< No mirroring.
        LEFT_RIGHT, ///< Mirroring along the left-right axis.
        FRONT_BACK ///< Mirroring along the front-back axis.
    };

    Type type; ///< The type of mirror.

    static const Mirror NONE; ///< No mirroring.
    static const Mirror LEFT_RIGHT; ///< Mirroring along the left-right axis.
    static const Mirror FRONT_BACK; ///< Mirroring along the front-back axis.

    /**
     * @brief Constructs a Mirror object with the specified type.
     * @param typeIn The type of mirror.
     */
    explicit Mirror(Type typeIn);

    /**
     * @brief Mirrors a rotation by a specified number of rotations.
     * @param rotationIn The rotation to mirror.
     * @param rotationCount The number of rotations to apply.
     * @return The mirrored rotation.
     */
    [[nodiscard]] int mirrorRotation(int rotationIn, int rotationCount) const;

    /**
     * @brief Converts a facing direction to a Rotation object based on the mirror.
     * @param facing The facing direction.
     * @return The corresponding Rotation object.
     */
    [[nodiscard]] Rotation toRotation(EnumFacing facing) const;

    /**
     * @brief Mirrors a facing direction.
     * @param facing The facing direction.
     * @return The mirrored facing direction.
     */
    [[nodiscard]] EnumFacing mirrorFACING(EnumFacing facing) const;

    /**
     * @brief Equality operator to compare two Mirror objects.
     * @param other The other Mirror object.
     * @return True if the mirrors are equal, false otherwise.
     */
    bool operator==(const Mirror &other) const;

    /**
     * @brief Gets all possible mirror values.
     * @return An array of the possible mirrors.
     */
    static std::array<Mirror, 3> values();
};

#include "RotationAndMirror.inl"
