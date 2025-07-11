#pragma once

#include <cmath>
#include <array>
#include "lce/processor.hpp"

/**
 * @class MathHelper
 * @brief Provides mathematical utility functions and precomputed sine table for optimized calculations.
 */
class MathHelper {
    static constexpr int TABLE_SIZE = 65536; ///< Size of the sine table.
    static constexpr int TABLE_WRAP = 65535; ///< Mask for wrapping sine table indices.


    // float SIN_TABLE[TABLE_SIZE]{}; ///< Precomputed sine values.
    static const std::array<float, TABLE_SIZE> SIN_TABLE;
    static constexpr float CONVERSION = 10430.378F; ///< Conversion factor for sine table indexing.


public:

    /**
     * @brief Computes the largest integer less than or equal to the given value.
     * @param value The input value.
     * @return The floored integer value.
     */
    static int floor(c_double value) {
        return static_cast<int>(std::floor(value));
    }

    /**
     * @brief Computes the sine of a value using the precomputed sine table.
     * @param value The input value in radians (range: 0-2π).
     * @return The sine of the input value.
     */
    static float sin(c_float value) {
        return SIN_TABLE[static_cast<int>(value * CONVERSION) & TABLE_WRAP];
    }

    /**
     * @brief Computes the cosine of a value using the precomputed sine table.
     * @param value The input value in radians (range: 0-2π).
     * @return The cosine of the input value.
     */
    static float cos(c_float value) {
        return SIN_TABLE[
                static_cast<int>(value * CONVERSION + 16384.0F) & TABLE_WRAP
        ];
    }

    /**
     * @brief Clamps a number to a specified range.
     * @param num The input number.
     * @param min The minimum value of the range.
     * @param max The maximum value of the range.
     * @return The clamped value.
     */
    static int clamp(c_int num, c_int min, c_int max) {
        if (num < min) { return min; }
        return num > max ? max : num;
    }

    /**
     * @brief Performs linear interpolation between two values.
     * @param ratio The interpolation ratio (0.0 to 1.0).
     * @param startValue The starting value.
     * @param endValue The ending value.
     * @return The interpolated value.
     */
    static double lerp(c_double ratio, c_double startValue, c_double endValue) {
        return startValue + ratio * (endValue - startValue);
    }

    /**
     * @brief Performs linear interpolation in two dimensions.
     * @param xRatio The interpolation ratio along the x-axis.
     * @param yRatio The interpolation ratio along the y-axis.
     * @param topLeft The value at the top-left corner.
     * @param topRight The value at the top-right corner.
     * @param bottomLeft The value at the bottom-left corner.
     * @param bottomRight The value at the bottom-right corner.
     * @return The interpolated value.
     */
    static double lerp2D(c_double xRatio, c_double yRatio, c_double topLeft,
                         c_double topRight, c_double bottomLeft, c_double bottomRight) {
        return lerp(yRatio, lerp(xRatio, topLeft, topRight), lerp(xRatio, bottomLeft, bottomRight));
    }

    /**
     * @brief Performs linear interpolation in three dimensions.
     * @param xRatio The interpolation ratio along the x-axis.
     * @param yRatio The interpolation ratio along the y-axis.
     * @param zRatio The interpolation ratio along the z-axis.
     * @param topFrontLeft The value at the top-front-left corner.
     * @param topFrontRight The value at the top-front-right corner.
     * @param topBackLeft The value at the top-back-left corner.
     * @param topBackRight The value at the top-back-right corner.
     * @param bottomFrontLeft The value at the bottom-front-left corner.
     * @param bottomFrontRight The value at the bottom-front-right corner.
     * @param bottomBackLeft The value at the bottom-back-left corner.
     * @param bottomBackRight The value at the bottom-back-right corner.
     * @return The interpolated value.
     */
    static double lerp3D(c_double xRatio, c_double yRatio, c_double zRatio,
                         double topFrontLeft, c_double topFrontRight,
                         c_double topBackLeft, c_double topBackRight, double bottomFrontLeft,
                         c_double bottomFrontRight, c_double bottomBackLeft, c_double bottomBackRight) {

        topFrontLeft = lerp2D(xRatio, yRatio, topFrontLeft, topFrontRight, topBackLeft, topBackRight);
        bottomFrontLeft = lerp2D(xRatio, yRatio, bottomFrontLeft, bottomFrontRight, bottomBackLeft, bottomBackRight);
        return lerp(zRatio, topFrontLeft, bottomFrontLeft);
    }

    /**
     * @brief Performs clamped linear interpolation between two values.
     * @param ratio The interpolation ratio (clamped between 0.0 and 1.0).
     * @param startValue The starting value.
     * @param endValue The ending value.
     * @return The interpolated value.
     */
    static double clampedLerp(c_double ratio, c_double startValue, c_double endValue) {
        if (ratio <= 0) return startValue;
        if (ratio >= 1) return endValue;
        return lerp(ratio, startValue, endValue);
    }
};