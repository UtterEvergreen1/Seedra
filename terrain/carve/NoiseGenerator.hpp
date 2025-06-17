#pragma once

#include <cmath>
#include <string>
#include <utility>
#include <vector>

#include "common/MathHelper.hpp"
#include "common/rng.hpp"
#include "terrain/generator.hpp"

#include <array>

/* * @brief Computes the floor of a double value.
 * @param value The input value.
 * @return The floor of the input value as a 64-bit integer.
 */
static i64 lfloor(c_double value) {
    c_auto i = (i64) value;
    return value < (double) i ? i - 1L : i;
}

/**
 * @class NoiseGeneratorSimplex
 * @brief Generates 2D simplex noise for terrain generation.
 */
class NoiseGeneratorSimplex {
public:
    int grad3[12][3] = {
        {1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0}, {1, 0, 1}, {-1, 0, 1},
        {1, 0, -1}, {-1, 0, -1}, {0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1}
    };
    double SQRT_3 = sqrt(3.0);
    int p[512]{};
    double xo{};
    double yo{};
    double zo{};
    double F2 = 0.5 * (SQRT_3 - 1.0);
    double G2 = (3.0 - SQRT_3) / 6.0;

    /**
     * @brief Initializes the simplex noise generator with a random seed.
     * @param rng The random number generator.
     */
    void setNoiseGeneratorSimplex(RNG &rng) {
        xo = rng.nextDouble() * 256.0;
        yo = rng.nextDouble() * 256.0;
        zo = rng.nextDouble() * 256.0;

        for (int i = 0; i < 256; i++) { p[i] = i; }

        for (int l = 0; l < 256; ++l) {
            c_int j = rng.nextInt(256 - l) + l;
            c_int k = p[l];
            p[l] = p[j];
            p[j] = k;
            p[l + 256] = p[l];
        }
    }

    /**
     * @brief Computes the floor of a value quickly.
     * @param value The input value.
     * @return The floor of the input value as an integer.
     */
    static int fastFloor(c_double value) { return value > 0.0 ? (int) value : (int) value - 1; }

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
    ND double getValue(c_double posX, c_double posZ) const {
        // Skewing and unskewing factors
        const double skewFactor = 0.5 * (SQRT_3 - 1.0);
        const double unskewFactor = (3.0 - SQRT_3) / 6.0;

        // Skew the input space
        double skew = (posX + posZ) * skewFactor;
        int skewedX = fastFloor(posX + skew);
        int skewedZ = fastFloor(posZ + skew);

        // Unskew the cell origin
        double unskew = (skewedX + skewedZ) * unskewFactor;
        double cellOriginX = skewedX - unskew;
        double cellOriginZ = skewedZ - unskew;

        // Distances from the cell origin
        double xDist = posX - cellOriginX;
        double zDist = posZ - cellOriginZ;

        // Determine simplex corner offsets
        int offsetX = (xDist > zDist) ? 1 : 0;
        int offsetZ = (xDist > zDist) ? 0 : 1;

        // Calculate the positions of the simplex corners
        double x1 = xDist - offsetX + unskewFactor;
        double z1 = zDist - offsetZ + unskewFactor;
        double x2 = xDist - 1.0 + 2.0 * unskewFactor;
        double z2 = zDist - 1.0 + 2.0 * unskewFactor;

        // Hash the coordinates to get gradient indices
        int permX = skewedX & 255;
        int permZ = skewedZ & 255;
        int gradIndex0 = p[permX + p[permZ]] % 12;
        int gradIndex1 = p[permX + offsetX + p[permZ + offsetZ]] % 12;
        int gradIndex2 = p[permX + 1 + p[permZ + 1]] % 12;

        // Calculate contributions from each corner
        double contribution0 = 0.5 - xDist * xDist - zDist * zDist;
        double noise0 = (contribution0 < 0) ? 0 : pow(contribution0, 4) * dot(grad3[gradIndex0], xDist, zDist);

        double contribution1 = 0.5 - x1 * x1 - z1 * z1;
        double noise1 = (contribution1 < 0) ? 0 : pow(contribution1, 4) * dot(grad3[gradIndex1], x1, z1);

        double contribution2 = 0.5 - x2 * x2 - z2 * z2;
        double noise2 = (contribution2 < 0) ? 0 : pow(contribution2, 4) * dot(grad3[gradIndex2], x2, z2);

        // Combine contributions and scale the result
        return 70.0 * (noise0 + noise1 + noise2);
    }

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
    std::vector<double> add(std::vector<double> noiseValues, double xOffset, double zOffset, int width,
                            int height, double xScale, double zScale, double noiseScale) {
        int index = 0;

        for (int z = 0; z < height; ++z) {
            double zCoord = (zOffset + z) * zScale + yo;

            for (int x = 0; x < width; ++x) {
                double xCoord = (xOffset + x) * xScale + xo;
                double skew = (xCoord + zCoord) * F2;
                int skewedX = fastFloor(xCoord + skew);
                int skewedZ = fastFloor(zCoord + skew);
                double unskew = (skewedX + skewedZ) * G2;
                double xUnskewed = xCoord - (skewedX - unskew);
                double zUnskewed = zCoord - (skewedZ - unskew);

                int offsetX, offsetZ;
                if (xUnskewed > zUnskewed) {
                    offsetX = 1;
                    offsetZ = 0;
                } else {
                    offsetX = 0;
                    offsetZ = 1;
                }

                double x1 = xUnskewed - offsetX + G2;
                double z1 = zUnskewed - offsetZ + G2;
                double x2 = xUnskewed - 1.0 + 2.0 * G2;
                double z2 = zUnskewed - 1.0 + 2.0 * G2;

                int permX = skewedX & 255;
                int permZ = skewedZ & 255;
                int gradIndex0 = p[permX + p[permZ]] % 12;
                int gradIndex1 = p[permX + offsetX + p[permZ + offsetZ]] % 12;
                int gradIndex2 = p[permX + 1 + p[permZ + 1]] % 12;

                double contribution0 = 0.5 - xUnskewed * xUnskewed - zUnskewed * zUnskewed;
                double noise0 = (contribution0 < 0)
                                    ? 0
                                    : pow(contribution0, 4) * dot(grad3[gradIndex0], xUnskewed, zUnskewed);

                double contribution1 = 0.5 - x1 * x1 - z1 * z1;
                double noise1 = (contribution1 < 0) ? 0 : pow(contribution1, 4) * dot(grad3[gradIndex1], x1, z1);

                double contribution2 = 0.5 - x2 * x2 - z2 * z2;
                double noise2 = (contribution2 < 0) ? 0 : pow(contribution2, 4) * dot(grad3[gradIndex2], x2, z2);

                noiseValues[index++] += 70.0 * (noise0 + noise1 + noise2) * noiseScale;
            }
        }
        return noiseValues;
    }
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
     * @brief Constructs a NoiseGeneratorPerlin with the specified number of levels.
     * @param rng The random number generator.
     * @param levelsIn The number of levels of noise to generate.
     */
    NoiseGeneratorPerlin(RNG rng, const int levelsIn) {
        this->setNoiseGeneratorPerlin(rng, levelsIn);
    }

    /**
     * @brief Initializes the Perlin noise generator with the specified number of levels.
     * @param rng The random number generator.
     * @param levelsIn The number of levels of noise to generate.
     */
    void setNoiseGeneratorPerlin(RNG &rng, const int levelsIn) {
        levels = levelsIn;
        noiseLevels = std::vector<NoiseGeneratorSimplex>(levelsIn);

        for (int i = 0; i < levelsIn; ++i) {
            noiseLevels[i].setNoiseGeneratorSimplex(rng);
        }
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
    void getRegion(std::vector<double> &noiseValues, double xOffset, double zOffset,
                   int width, int height, double xScale, double zScale,
                   double persistence, double lacunarity) {
        // Ensure the vector is properly sized and initialized
        if (noiseValues.size() < static_cast<size_t>(width * height)) {
            noiseValues.assign(width * height, 0.0);
        } else {
            std::fill(noiseValues.begin(), noiseValues.end(), 0.0);
        }

        double amplitude = 1.0;
        double frequency = 1.0;

        // Add noise from each level
        for (int i = 0; i < levels; ++i) {
            noiseValues = noiseLevels[i].add(noiseValues, xOffset, zOffset, width, height,
                                             xScale * frequency, zScale * frequency, 0.55 / amplitude);
            frequency *= lacunarity;
            amplitude *= persistence;
        }
    }

    /**
      * @brief Overloaded method to generate a region of Perlin noise values with default lacunarity.
      * @param noiseValues The vector to store noise values.
      * @param xOffset The X-offset of the region.
      * @param zOffset The Z-offset of the region.
      * @param width The width of the region.
      * @param height The height of the region.
      * @param xScale The X-scale of the noise.
      * @param zScale The Z-scale of the noise.
      * @param persistence The persistence factor for scaling.
      */
    void getRegion(std::vector<double> &noiseValues, double xOffset, double zOffset,
                   int width, int height, double xScale, double zScale, double persistence) {
        // Call the main getRegion method with default lacunarity value of 0.5
        getRegion(noiseValues, xOffset, zOffset, width, height, xScale, zScale, persistence, 0.5);
    }
};

/**
 * @class NoiseGeneratorImproved
 * @brief Generates improved Perlin noise for 3D terrain generation.
 */
class NoiseGeneratorImproved {
    /**
     * @brief A static constexpr array containing integers from 0 to 255.
     * Used for initializing the permutation array.
     */
    static constexpr std::array<int, 256> oneTo255Array = []() constexpr {
        std::array<int, 256> arr = {};
        for (int i = 0; i < 256; ++i) {
            arr[i] = i;
        }
        return arr;
    }();

public:
    /**
     * @brief Permutation array used for noise generation.
     * The first 256 elements are shuffled numbers from 0 to 255, and the second half is a duplicate of the first.
     */
    int permutations[512]{};

    /**
     * @brief Coordinates used for noise generation.
     */
    double xCoord{};
    double yCoord{};
    double zCoord{};

    /**
     * @brief Gradient vectors for 3D noise generation.
     */
    double GRAD_X[16] = {1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};
    double GRAD_Y[16] = {1.0, 1.0, -1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0};
    double GRAD_Z[16] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 0.0, 1.0, 0.0, -1.0};
    double GRAD_2X[16] = {1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};
    double GRAD_2Z[16] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 0.0, 1.0, 0.0, -1.0};

    /**
     * @brief Initializes the noise generator with a random seed.
     * @param rng The random number generator used for shuffling the permutation array.
     */
    void setNoiseGeneratorImproved(RNG &rng) {
        xCoord = rng.nextDouble() * 256.0;
        yCoord = rng.nextDouble() * 256.0;
        zCoord = rng.nextDouble() * 256.0;

        // Initialize the first 256 elements of the permutation array.
        memcpy(permutations, oneTo255Array.data(), 256 * sizeof(int));

        // Shuffle the permutation array and duplicate it.
        for (int l = 0; l < 256; ++l) {
            int j = rng.nextInt(256 - l) + l;
            int k = permutations[l];
            permutations[l] = permutations[j];
            permutations[j] = k;
            permutations[l + 256] = permutations[l];
        }
    }

    /**
     * @brief Computes the gradient dot product for 3D noise.
     * @param hash The hash value used to select the gradient.
     * @param x The X-coordinate of the input vector.
     * @param y The Y-coordinate of the input vector.
     * @param z The Z-coordinate of the input vector.
     * @return The dot product of the gradient and the input vector.
     */
    ND double grad(int hash, double x, double y, double z) const {
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
    ND double grad2(int hash, double x, double z) const {
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
    std::vector<double> populateNoiseArray(const Generator *g, std::vector<double> noiseArray, double xOffset, double yOffset,
                                           double zOffset, int xSize, int ySize, int zSize, double xScale,
                                           double yScale, double zScale, double noiseScale) {
        if (ySize == 1) {
            // Special case for 2D noise generation.
            int i5 = 0;
            int j5 = 0;
            int j = 0;
            int k5 = 0;
            double d14 = 0.0;
            double d15 = 0.0;
            int l5 = 0;
            double d16 = 1.0 / noiseScale;

            double d17 = xOffset + xCoord;
            for (int j2 = 0; j2 < xSize; ++j2) {
                int i6 = (int) d17;

                if (d17 < (double) i6) { --i6; }

                int k2 = i6 & 255;
                if (g->getConsole() != lce::CONSOLE::WIIU) {
                    d17 = d17 - (double) i6;
                }
                double d18 = d17 * d17 * d17 * (d17 * (d17 * 6.0 - 15.0) + 10.0);

                for (int j6 = 0; j6 < zSize; ++j6) {
                    double d19 = zOffset + (double) j6 * zScale + zCoord;
                    int k6 = (int) d19;

                    if (d19 < (double) k6) { --k6; }

                    int l6 = k6 & 255;
                    if (g->getConsole() != lce::CONSOLE::WIIU) {
                        d19 = d19 - (double) k6;
                    }
                    double d20 = d19 * d19 * d19 * (d19 * (d19 * 6.0 - 15.0) + 10.0);
                    i5 = permutations[k2] + 0;
                    j5 = permutations[i5] + l6;
                    j = permutations[k2 + 1] + 0;
                    k5 = permutations[j] + l6;
                    d14 = MathHelper::lerp(d18, grad2(permutations[j5], d17, d19),
                                           grad(permutations[k5], d17 - 1.0, 0.0, d19));
                    d15 = MathHelper::lerp(d18, grad(permutations[j5 + 1], d17, 0.0, d19 - 1.0),
                                           grad(permutations[k5 + 1], d17 - 1.0, 0.0, d19 - 1.0));
                    double d21 = MathHelper::lerp(d20, d14, d15);
                    int i7 = l5++;
                    noiseArray[i7] += d21 * d16;
                }
                d17 += i6 + xScale;
            }
        } else {
            // General case for 3D noise generation.
            int i = 0;
            double d0 = 1.0 / noiseScale;
            int k = -1;
            int l = 0;
            int i1 = 0;
            int j1 = 0;
            int k1 = 0;
            int l1 = 0;
            int i2 = 0;
            double d1 = 0.0;
            double d2 = 0.0;
            double d3 = 0.0;
            double d4 = 0.0;

            for (int l2 = 0; l2 < xSize; ++l2) {
                double d5 = xOffset + (double) l2 * xScale + xCoord;
                int i3 = (int) d5;

                if (d5 < (double) i3) { --i3; }

                int j3 = i3 & 255;
                d5 = d5 - (double) i3;
                double d6 = d5 * d5 * d5 * (d5 * (d5 * 6.0 - 15.0) + 10.0);

                for (int k3 = 0; k3 < zSize; ++k3) {
                    double d7 = zOffset + (double) k3 * zScale + zCoord;
                    int l3 = (int) d7;

                    if (d7 < (double) l3) { --l3; }

                    int i4 = l3 & 255;
                    d7 = d7 - (double) l3;
                    double d8 = d7 * d7 * d7 * (d7 * (d7 * 6.0 - 15.0) + 10.0);

                    for (int j4 = 0; j4 < ySize; j4++) {
                        double d9 = yOffset + (double) j4 * yScale + yCoord;
                        int k4 = (int) d9;

                        if (d9 < (double) k4) { --k4; }

                        int l4 = k4 & 255;
                        d9 = d9 - (double) k4;
                        double d10 = d9 * d9 * d9 * (d9 * (d9 * 6.0 - 15.0) + 10.0);

                        if (j4 == 0 || l4 != k) {
                            k = l4;
                            l = permutations[j3] + l4;
                            i1 = permutations[l] + i4;
                            j1 = permutations[l + 1] + i4;
                            k1 = permutations[j3 + 1] + l4;
                            l1 = permutations[k1] + i4;
                            i2 = permutations[k1 + 1] + i4;
                            d1 = MathHelper::lerp(d6, grad(permutations[i1], d5, d9, d7),
                                                  grad(permutations[l1], d5 - 1.0, d9, d7));
                            d2 = MathHelper::lerp(d6, grad(permutations[j1], d5, d9 - 1.0, d7),
                                                  grad(permutations[i2], d5 - 1.0, d9 - 1.0, d7));
                            d3 = MathHelper::lerp(d6, grad(permutations[i1 + 1], d5, d9, d7 - 1.0),
                                                  grad(permutations[l1 + 1], d5 - 1.0, d9, d7 - 1.0));
                            d4 = MathHelper::lerp(d6, grad(permutations[j1 + 1], d5, d9 - 1.0, d7 - 1.0),
                                                  grad(permutations[i2 + 1], d5 - 1.0, d9 - 1.0, d7 - 1.0));
                        }

                        double d11 = MathHelper::lerp(d10, d1, d2);
                        double d12 = MathHelper::lerp(d10, d3, d4);
                        double d13 = MathHelper::lerp(d8, d11, d12);
                        int j7 = i++;
                        noiseArray[j7] += d13 * d0;
                    }
                }
            }
        }
        return noiseArray;
    }
};

/**
 * @class NoiseGeneratorOctaves
 * @brief Combines multiple noise generation functions to produce octaves of noise.
 */
class NoiseGeneratorOctaves {
public:
    /**
     * @brief Collection of noise generation functions.
     * Each function contributes to a different octave of noise.
     */
    std::vector<NoiseGeneratorImproved> generatorCollection;

    /**
     * @brief The number of octaves used in noise generation.
     */
    int octaves;

    /**
     * @brief Initializes the noise generator with the specified number of octaves.
     * @param rng The random number generator used for initializing each octave.
     * @param octavesIn The number of octaves to generate.
     */
    void setNoiseGeneratorOctaves(RNG &rng, c_int octavesIn) {
        octaves = octavesIn;
        generatorCollection = std::vector<NoiseGeneratorImproved>(octavesIn);

        for (int i = 0; i < octavesIn; ++i) {
            generatorCollection[i].setNoiseGeneratorImproved(rng);
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
    std::vector<double> genNoiseOctaves(const Generator *g, std::vector<double> noiseArray, c_int xOffset, c_int yOffset,
                                        c_int zOffset, c_int xSize, c_int ySize, c_int zSize,
                                        c_double xScale, c_double yScale, c_double zScale) {
        // Ensure the noise array is properly sized and initialized
        if EXPECT_TRUE(noiseArray.empty()) {
            noiseArray.resize(xSize * ySize * zSize, 0.0);
        } else {
            std::fill(noiseArray.begin(), noiseArray.end(), 0.0);
        }

        double amplitude = 1.0;

        // Generate noise for each octave
        for (int i = 0; i < octaves; ++i) {
            double scaledXOffset = xOffset * amplitude * xScale;
            double scaledYOffset = yOffset * amplitude * yScale;
            double scaledZOffset = zOffset * amplitude * zScale;

            i64 flooredX = lfloor(scaledXOffset);
            i64 flooredZ = lfloor(scaledZOffset);

            scaledXOffset -= flooredX;
            scaledZOffset -= flooredZ;

            flooredX %= 16777216LL;
            flooredZ %= 16777216LL;

            scaledXOffset += flooredX;
            scaledZOffset += flooredZ;

            noiseArray = generatorCollection[i].populateNoiseArray(g, noiseArray, scaledXOffset, scaledYOffset,
                                                                   scaledZOffset,
                                                                   xSize, ySize, zSize, xScale * amplitude,
                                                                   yScale * amplitude,
                                                                   zScale * amplitude, amplitude);

            amplitude /= 2.0;
        }

        return noiseArray;
    }

    /**
     * @brief Overloaded method to generate noise values for a 2D region with default parameters.
     * @param g The generator object used for console-specific behavior.
     * @param noiseArray The array to store the generated noise values.
     * @param xOffset The X-offset for noise generation.
     * @param zOffset The Z-offset for noise generation.
     * @param xSize The size of the region in the X-dimension.
     * @param zSize The size of the region in the Z-dimension.
     * @param xScale The scale factor for the X-dimension.
     * @param zScale The scale factor for the Z-dimension.
     * @return A vector containing the generated noise values.
     */
    std::vector<double> genNoiseOctaves(const Generator *g, std::vector<double> noiseArray, c_int xOffset, c_int zOffset,
                                        c_int xSize, c_int zSize, c_double xScale, c_double zScale) {
        return genNoiseOctaves(g, std::move(noiseArray), xOffset, 10, zOffset, xSize, 1, zSize, xScale, 1.0, zScale);
    }
};
