#pragma once

#include <cmath>
#include <string>
#include <utility>
#include <vector>

#include "common/MathHelper.hpp"
#include "common/rng.hpp"
#include "noise.hpp"

#include <array>


class Generator;


namespace noise_values {
    // Skewing and unskewing factors (2D simplex)
    static constexpr double SQRT_3 = 1.7320508075688772; // sqrt(3.0);
    static constexpr double NOISE_SIMPLEX_SKEW_FACTOR = 0.5 * (SQRT_3 - 1.0);
    static constexpr double NOISE_SIMPLEX_UNSKEW_FACTOR = (3.0 - SQRT_3) / 6.0;

    static constexpr int NOISE_SIMPLEX_GRAD3[12][3] = {
            {1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0}, {1, 0, 1}, {-1, 0, 1},
            {1, 0, -1}, {-1, 0, -1}, {0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1}
    };



    /**
     * @brief Computes the gradient dot product for 3D noise.
     * @param hash The hash value used to select the gradient.
     * @param x The X-coordinate of the input vector.
     * @param y The Y-coordinate of the input vector.
     * @param z The Z-coordinate of the input vector.
     * @return The dot product of the gradient and the input vector.
     */
    ND static double grad(c_int hash, c_double x, c_double y, c_double z) {
        static constexpr double GRAD_X[16] = { 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0 };
        static constexpr double GRAD_Y[16] = { 1.0, 1.0, -1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0 };
        static constexpr double GRAD_Z[16] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 0.0, 1.0, 0.0, -1.0 };

        c_int i = hash & 15;
        return GRAD_X[i] * x + GRAD_Y[i] * y + GRAD_Z[i] * z;
    }

    /**
     * @brief Computes the gradient dot product for 2D noise.
     * @param hash The hash value used to select the gradient.
     * @param x The X-coordinate of the input vector.
     * @param z The Z-coordinate of the input vector.
     * @return The dot product of the gradient and the input vector.
     */
    ND static double grad2(c_int hash, c_double x, c_double z) {
        static constexpr double GRAD_2X[16] = { 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0 };
        static constexpr double GRAD_2Z[16] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 0.0, 1.0, 0.0, -1.0 };

        c_int i = hash & 15;
        return GRAD_2X[i] * x + GRAD_2Z[i] * z;
    }


    /**
     * @brief Computes the floor of a value quickly.
     * @param value The input value.
     * @return The floor of the input value as an integer.
     */
    static int fastFloor(c_double value) {
        return value > 0.0 ? (int) value : (int) value - 1;
    }

    /**
      * @brief Computes the dot product of a gradient vector and a 2D vector.
      * @param gradient The gradient vector.
      * @param x The X-coordinate of the vector.
      * @param y The Y-coordinate of the vector.
      * @return The dot product.
      */
    static double dot(const int gradient[], double x, double y) {
        return gradient[0] * x + gradient[1] * y;
    }


}





/**
 * @class NoiseGeneratorSimplex
 * @brief Generates 2D simplex noise for terrain generation.
 */
class NoiseGeneratorSimplex : public PerlinNoise {
public:
    ND double getValue(c_double posX, c_double posZ) const;

    template<int Width, int Height>
    void add(std::array<double, Width * Height> &noiseValues,
             double xOffset, double zOffset,
             double xScale, double zScale, double noiseScale) const;
};


/**
 * @class NoiseGeneratorPerlin
 * @brief Generates Perlin noise using multiple levels of simplex noise.
 */
template<int Levels>
class NoiseGeneratorPerlin {
public:

    /// @brief a vector of simplex noise generators for each level.
    std::array<NoiseGeneratorSimplex, Levels> noiseLevels;

    NoiseGeneratorPerlin() = default;

    explicit NoiseGeneratorPerlin(RNG rng) {
        setNoiseGeneratorPerlin(rng);
    }

    /**
     * @brief Initializes the Perlin noise generator with the specified number of levels.
     * @param rng The random number generator.
     */
    void setNoiseGeneratorPerlin(RNG &rng) {
        noiseLevels = {};
        for (int i = 0; i < Levels; ++i) {
            perlinInit(&noiseLevels[i], rng);
        }
    }

    /**
     * @brief Computes the Perlin noise value at a given 2D position.
     * @param posX The X-coordinate of the position.
     * @param posZ The Z-coordinate of the position.
     * @return The noise value at the given position.
     */
    ND double getValue(c_double posX, c_double posZ) const {
        double d0 = 0.0, d1 = 1.0;
        for (int i = 0; i < Levels; ++i) {
            d0 += noiseLevels[i].getValue(posX * d1, posZ * d1) / d1;
            d1 /= 2.0;
        }
        return d0;
    }

    /**
     * @brief Generates a region of Perlin noise values.
     * @param console The console type (LCE console).
     * @param noiseValues The vector to store noise values.
     * @param xOffset The X-offset of the region.
     * @param zOffset The Z-offset of the region.
     * @tparam Width The width of the region.
     * @tparam Height The height of the region.
     * @tparam XScale The X-scale of the noise.
     * @tparam ZScale The Z-scale of the noise.
     * @tparam Persistence The persistence factor for scaling.
     * @tparam Lacunarity The lacunarity factor for scaling.
     */
    template<int Width, int Height, double XScale, double ZScale, double Persistence = 0.5, double Lacunarity = 2.0>
    void getRegion(lce::CONSOLE console, std::array<double, Width * Height> &noiseValues, double xOffset, double zOffset) const {
        memset(noiseValues.data(), 0, sizeof(noiseValues));
        double amplitude = 1.0, frequency = 1.0;
        bool isWiiU = console == lce::CONSOLE::WIIU;

        // Add noise from each level
        for (int i = 0; i < Levels; ++i) {
            c_double coordinateMultiplier = isWiiU ? (frequency / 1.5) : (frequency * amplitude);
            noiseLevels[i].template add<Width, Height>(noiseValues, xOffset, zOffset,
                               XScale * coordinateMultiplier, ZScale * coordinateMultiplier, 0.55 / amplitude);
            frequency *= Lacunarity;
            amplitude *= Persistence;
        }
    }
};

/**
 * @class NoiseGeneratorImproved
 * @brief Generates improved Perlin noise for 3D terrain generation.
 */
class NoiseGeneratorImproved : public PerlinNoise {
public:

    template<int XSize, int YSize, int ZSize>
    void add(const Generator *g, std::array<double, XSize * YSize * ZSize> &noiseArray,
                                double xOffset, double yOffset, double zOffset,
                                double xScale, double yScale, double zScale, double noiseScale) const;
};

/**
 * @class NoiseGeneratorOctaves
 * @brief Combines multiple noise generation functions to produce octaves of noise.
 */
template<int Octaves>
class NoiseGeneratorOctaves {
    /**
     * @brief Collection of noise generation functions.
     * Each function contributes to a different octave of noise.
     */
    std::array<NoiseGeneratorImproved, Octaves> generatorCollection;
public:

    /**
     * @brief Initializes the noise generator with the specified number of octaves.
     * @param rng The random number generator used for initializing each octave.
     */
    void setNoiseGeneratorOctaves(RNG &rng) {
        generatorCollection = {};
        for (int i = 0; i < Octaves; ++i) {
            perlinInit(&generatorCollection[i], rng);
        }
    }

    template<typename Container, int XSize, int YSize, int ZSize, double XScale, double YScale, double ZScale>
    void getRegion(const Generator *g,
                             std::array<Container, XSize * YSize * ZSize> &noiseArray,
                             int xOffset, int yOffset, int zOffset) const;
};



struct ChunkNoise {
    bool initialized = false; ///< Indicates if the noise generators have been initialized.
    RNG rng; ///< The RNG state after setting up the scales.
    NoiseGeneratorOctaves<16> minLimitPerlinNoise; ///< Noise generator for minimum limit Perlin noise.
    NoiseGeneratorOctaves<16> maxLimitPerlinNoise; ///< Noise generator for maximum limit Perlin noise.
    NoiseGeneratorOctaves<8> mainPerlinNoise; ///< Noise generator for main Perlin noise.
    NoiseGeneratorPerlin<4> surfaceNoise; ///< Noise generator for surface noise.
    NoiseGeneratorOctaves<10> scaleNoise; ///< Noise generator for scale noise (currently unused).
    NoiseGeneratorOctaves<16> depthNoise; ///< Noise generator for depth noise.
};
