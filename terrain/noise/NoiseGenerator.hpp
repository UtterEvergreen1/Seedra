#pragma once

#include <cmath>
#include <string>
#include <utility>
#include <vector>

#include "common/MathHelper.hpp"
#include "common/rng.hpp"
#include "terrain/generator.hpp"
#include "noise.hpp"

#include <array>


/**
 * @class NoiseGeneratorSimplex
 * @brief Generates 2D simplex noise for terrain generation.
 */
class NoiseGeneratorSimplex : public PerlinNoise {
    static constexpr int GRAD3[12][3] = {
        {1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0}, {1, 0, 1}, {-1, 0, 1},
        {1, 0, -1}, {-1, 0, -1}, {0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1}
    };

public:

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

    /**
     * @brief Generates simplex noise for a given 2D position.
     * @param posX The X-coordinate of the position.
     * @param posZ The Z-coordinate of the position.
     * @return The noise value at the given position.
     */
    ND double getValue(c_double posX, c_double posZ) const;

    /**
     * @brief Adds simplex noise to a region.
     * @param noiseValues The vector to store noise values.
     * @param xOffset The X-offset.
     * @param zOffset The Z-offset.
     * @param width The width of the region.
     * @param height The height of the region.
     * @param xScale The X-scale.
     * @param zScale The Z-scale.
     * @param noiseScale The noise scale.
     * @return The vector with added noise values.
     */
    void add(std::vector<double>& noiseValues,
             double xOffset, double zOffset, int width, int height,
             double xScale, double zScale, double noiseScale);
};


/**
 * @class NoiseGeneratorPerlin
 * @brief Generates Perlin noise using multiple levels of simplex noise.
 */
class NoiseGeneratorPerlin {
public:
    /**
     * @brief A vector of simplex noise generators for each level.
     */
    std::vector<NoiseGeneratorSimplex> noiseLevels;

    /**
     * @brief The number of levels of noise.
     */
    int levels{};

    /**
     * @brief Default constructor for NoiseGeneratorPerlin.
     */
    NoiseGeneratorPerlin() = default;

    /**
     * @brief Initializes the Perlin noise generator with the specified number of levels.
     * @param rng The random number generator.
     * @param levelsIn The number of levels of noise to generate.
     */
    void setNoiseGeneratorPerlin(RNG &rng, int levelsIn);

    NoiseGeneratorPerlin(RNG rng, const int levelsIn) {
        this->setNoiseGeneratorPerlin(rng, levelsIn);
    }


    /**
     * @brief Computes the Perlin noise value at a given 2D position.
     * @param posX The X-coordinate of the position.
     * @param posZ The Z-coordinate of the position.
     * @return The noise value at the given position.
     */
    ND double getValue(const double posX, const double posZ) const {
        double d0 = 0.0;
        double d1 = 1.0;

        for (int i = 0; i < levels; ++i) {
            d0 += noiseLevels[i].getValue(posX * d1, posZ * d1) / d1;
            d1 /= 2.0;
        }

        return d0;
    }

    /**
     * @brief Generates a region of Perlin noise values.
     * @param noiseValues The vector to store noise values.
     * @param xOffset The X-offset of the region.
     * @param zOffset The Z-offset of the region.
     * @param width The width of the region.
     * @param height The height of the region.
     * @param xScale The X-scale of the noise.
     * @param zScale The Z-scale of the noise.
     * @param persistence The persistence factor for scaling.
     * @param lacunarity The lacunarity factor for scaling.
     */
    void getRegion(std::vector<double>& noiseValues, double xOffset, double zOffset,
                   int width, int height, double xScale, double zScale,
                   double persistence, double lacunarity = 0.5) {
        // Ensure the vector is properly sized and initialized
        if (noiseValues.size() < (u64)width * (u64)height) {
            noiseValues.assign(width * height, 0.0);
        } else {
            std::fill(noiseValues.begin(), noiseValues.end(), 0.0);
        }

        double amplitude = 1.0;
        double frequency = 1.0;

        // Add noise from each level
        for (int i = 0; i < levels; ++i) {
            noiseLevels[i].add(noiseValues, xOffset, zOffset, width, height,
                               xScale * frequency, zScale * frequency, 0.55 / amplitude);
            frequency *= lacunarity;
            amplitude *= persistence;
        }
    }

};

/**
 * @class NoiseGeneratorImproved
 * @brief Generates improved Perlin noise for 3D terrain generation.
 */
class NoiseGeneratorImproved : public PerlinNoise {
public:

    /**
     * @brief Computes the gradient dot product for 3D noise.
     * @param hash The hash value used to select the gradient.
     * @param x The X-coordinate of the input vector.
     * @param y The Y-coordinate of the input vector.
     * @param z The Z-coordinate of the input vector.
     * @return The dot product of the gradient and the input vector.
     */
    ND static double grad(int hash, double x, double y, double z) {
        static constexpr double GRAD_X[16] = {1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};
        static constexpr double GRAD_Y[16] = {1.0, 1.0, -1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0};
        static constexpr double GRAD_Z[16] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 0.0, 1.0, 0.0, -1.0};

        int i = hash & 15;
        return GRAD_X[i] * x + GRAD_Y[i] * y + GRAD_Z[i] * z;
    }

    /**
     * @brief Computes the gradient dot product for 2D noise.
     * @param hash The hash value used to select the gradient.
     * @param x The X-coordinate of the input vector.
     * @param z The Z-coordinate of the input vector.
     * @return The dot product of the gradient and the input vector.
     */
    ND static double grad2(int hash, double x, double z) {
        static constexpr double GRAD_2X[16] = {1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};
        static constexpr double GRAD_2Z[16] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 0.0, 1.0, 0.0, -1.0};

        int i = hash & 15;
        return GRAD_2X[i] * x + GRAD_2Z[i] * z;
    }

    /**
     * @brief Populates a noise array with generated noise values.
     * @param g The generator object used for console-specific behavior.
     * @param noiseArray The array to store the generated noise values.
     * @param xOffset The X-offset for noise generation.
     * @param yOffset The Y-offset for noise generation.
     * @param zOffset The Z-offset for noise generation.
     * @param xSize The size of the noise array in the X-dimension.
     * @param ySize The size of the noise array in the Y-dimension.
     * @param zSize The size of the noise array in the Z-dimension.
     * @param xScale The scale factor for the X-dimension.
     * @param yScale The scale factor for the Y-dimension.
     * @param zScale The scale factor for the Z-dimension.
     * @param noiseScale The overall scale factor for the noise.
     * @return The populated noise array.
     */
    void populateNoiseArray(const Generator *g, std::vector<double>& noiseArray,
                            double xOffset, double yOffset, double zOffset,
                            int xSize, int ySize, int zSize, double xScale,
                            double yScale, double zScale, double noiseScale);
};

/**
 * @class NoiseGeneratorOctaves
 * @brief Combines multiple noise generation functions to produce octaves of noise.
 */
class NoiseGeneratorOctaves {
    /**
     * @brief Collection of noise generation functions.
     * Each function contributes to a different octave of noise.
     */
    std::vector<NoiseGeneratorImproved> generatorCollection;

    /**
     * @brief The number of octaves used in noise generation.
     */
    int octaves;
public:

    /**
     * @brief Initializes the noise generator with the specified number of octaves.
     * @param rng The random number generator used for initializing each octave.
     * @param octavesIn The number of octaves to generate.
     */
    void setNoiseGeneratorOctaves(RNG &rng, c_int octavesIn) {
        octaves = octavesIn;
        generatorCollection = std::vector<NoiseGeneratorImproved>(octavesIn);

        for (int i = 0; i < octavesIn; ++i) {
            perlinInit(&generatorCollection[i], rng);
        }
    }

    /**
     * @brief Generates noise values for a 3D region using multiple octaves.
     * @param g The generator object used for console-specific behavior.
     * @param noiseArray The array to store the generated noise values.
     * @param xOffset The X-offset for noise generation.
     * @param yOffset The Y-offset for noise generation.
     * @param zOffset The Z-offset for noise generation.
     * @param xSize The size of the region in the X-dimension.
     * @param ySize The size of the region in the Y-dimension.
     * @param zSize The size of the region in the Z-dimension.
     * @param xScale The scale factor for the X-dimension.
     * @param yScale The scale factor for the Y-dimension.
     * @param zScale The scale factor for the Z-dimension.
     * @return A vector containing the generated noise values.
     */
    void genNoiseOctaves(const Generator *g,
                         std::vector<double>& noiseArray,
                         c_int xOffset, c_int yOffset, c_int zOffset,
                         c_int xSize, c_int ySize, c_int zSize,
                         c_double xScale, c_double yScale, c_double zScale);

};
