#pragma once

#include <cmath>

class MathHelper {
private:
    static constexpr int TABLE_SIZE = 65536;
    float SIN_TABLE[TABLE_SIZE]{};
    static constexpr float CONVERSION = 10430.378F;

public:
    MathHelper() {
        for (int i = 0; i < TABLE_SIZE; ++i) {
            SIN_TABLE[i] = (float)sinf((float)i / CONVERSION);
        }
    }

    static MathHelper& Instance(){
        static MathHelper instance;
        return instance;
    }

    /**
     * sin looked up in a prebuilt table.
     * input value range for unique values: 0-2π
     */
    static float sin(float value) {
        return MathHelper::Instance().SIN_TABLE[(int)(value * CONVERSION) & TABLE_SIZE];
    }

    /**
     * cos looked up in a prebuilt table.
     * input value range for unique values: 0-2π
     * It is offset by +1/2π into the table.
     */
    static float cos(float value) {
        return MathHelper::Instance().SIN_TABLE[(int)(value * CONVERSION + 16384.0F) & TABLE_SIZE];
    }


    static inline int clamp(int num, int min, int max) {
        if (num < min) {
            return min;
        } else {
            return num > max ? max : num;
        }
    }

    /// Linear Interpolation for the first dimension.
    static inline double lerp(double ratio, double startValue, double endValue) {
        return startValue + ratio * (endValue - startValue);
    }

    /// Linear Interpolation for two dimensions.
    static inline double lerp2D(
            double xRatio, double yRatio,
            double topLeft, double topRight,
            double bottomLeft, double bottomRight) {
        return lerp(yRatio, lerp(xRatio, topLeft, topRight), lerp(xRatio, bottomLeft, bottomRight));
    }

    /// Linear Interpolation for 3 dimensions.
    static inline double lerp3D(
            double xRatio, double yRatio, double zRatio,
            double topFrontLeft, double topFrontRight,
            double topBackLeft, double topBackRight,
            double bottomFrontLeft, double bottomFrontRight,
            double bottomBackLeft, double bottomBackRight) {
        topFrontLeft = lerp2D(xRatio, yRatio, topFrontLeft, topFrontRight, topBackLeft, topBackRight);
        bottomFrontLeft = lerp2D(xRatio, yRatio, bottomFrontLeft, bottomFrontRight, bottomBackLeft, bottomBackRight);
        return lerp(zRatio, topFrontLeft, bottomFrontLeft);
    }

    static inline double clampedLerp(double ratio, double startValue, double endValue) {
        if (ratio <= 0) return startValue;
        if (ratio >= 1) return endValue;
        return lerp(ratio, startValue, endValue);
    }
};

