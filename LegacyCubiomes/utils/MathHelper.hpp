#pragma once

#include <cmath>

#include "lce/processor.hpp"


class MathHelper {
    static constexpr int TABLE_SIZE = 65536;
    static constexpr int TABLE_WRAP = 65535;
    float SIN_TABLE[TABLE_SIZE]{};
    static constexpr float CONVERSION = 10430.378F;

public:
    MathHelper() {
        for (int i = 0; i < TABLE_SIZE; ++i) {
            SIN_TABLE[i] = sinf(static_cast<float>(i) / CONVERSION);
        }
    }

    static MathHelper& Instance() {
        static MathHelper instance;
        return instance;
    }

    /**
     * sin looked up in a prebuilt table.
     * input value range for unique values: 0-2π
     */
    static float sin(c_float value) {
        return Instance().SIN_TABLE[static_cast<int>(value * CONVERSION) & TABLE_WRAP];
    }

    /**
     * cos looked up in a prebuilt table.
     * input value range for unique values: 0-2π
     * It is offset by +1/2π into the table.
     */
    static float cos(c_float value) {
        return Instance().SIN_TABLE[static_cast<int>(value * CONVERSION + 16384.0F) & TABLE_WRAP];
    }


    static int clamp(c_int num, c_int min, c_int max) {
        if (num < min) { return min; }
        return num > max ? max : num;
    }

    /// Linear Interpolation for the first dimension.
    static double lerp(c_double ratio, c_double startValue, c_double endValue) {
        return startValue + ratio * (endValue - startValue);
    }

    /// Linear Interpolation for two dimensions.
    static double lerp2D(c_double xRatio, c_double yRatio, c_double topLeft,
        c_double topRight, c_double bottomLeft, c_double bottomRight) {
        return lerp(yRatio, lerp(xRatio, topLeft, topRight), lerp(xRatio, bottomLeft, bottomRight));
    }

    /// Linear Interpolation for 3 dimensions.
    static double lerp3D(c_double xRatio, c_double yRatio, c_double zRatio,
        double topFrontLeft, c_double topFrontRight,
        c_double topBackLeft, c_double topBackRight, double bottomFrontLeft,
        c_double bottomFrontRight, c_double bottomBackLeft, c_double bottomBackRight) {
        topFrontLeft = lerp2D(xRatio, yRatio, topFrontLeft, topFrontRight, topBackLeft, topBackRight);
        bottomFrontLeft = lerp2D(xRatio, yRatio, bottomFrontLeft, bottomFrontRight, bottomBackLeft, bottomBackRight);
        return lerp(zRatio, topFrontLeft, bottomFrontLeft);
    }

    static double clampedLerp(c_double ratio, c_double startValue, c_double endValue) {
        if (ratio <= 0) return startValue;
        if (ratio >= 1) return endValue;
        return lerp(ratio, startValue, endValue);
    }
};
