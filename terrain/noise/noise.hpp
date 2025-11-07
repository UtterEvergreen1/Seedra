#pragma once

#include "lce/enums.hpp"
#include "common/rng.hpp"

/**
 * @class Generator
 * @brief Forward declaration of the Generator class.
 */
class Generator;



/**
 * @brief Maintains precision for a given double value.
 *
 * This function ensures that the precision of the input value is preserved
 * during calculations.
 *
 * @param x The input double value.
 * @return The double value with maintained precision.
 */
double maintainPrecision(double x);

/// Perlin noise

/**
 * @struct PerlinNoise
 * @brief Represents Perlin noise used for terrain generation.
 *
 * This structure contains the parameters and data required to generate
 * Perlin noise, which is commonly used in procedural terrain generation.
 */
struct PerlinNoise {
    u8 permutations[512]; ///< Permutation table for Perlin noise.
    double x;             ///< Parameter 'x' for noise generation.
    double y;             ///< Parameter 'y' for noise generation.
    double z;             ///< Parameter 'z' for noise generation.
    double amplitude;     ///< Amplitude of the noise.
    double lacunarity;    ///< Lacunarity of the noise.
};


/**
 * @struct OctaveNoise
 * @brief Represents octave noise, which combines multiple layers of Perlin noise.
 *
 * This structure is used to generate more complex noise patterns by combining
 * multiple Perlin noise layers (octaves).
 */
struct OctaveNoise {
    int octcnt{};                   ///< Number of octaves.
    PerlinNoise* octaves = nullptr; ///< Pointer to an array of PerlinNoise structures.
};


/**
 * @struct SurfaceNoise
 * @brief Represents surface noise, which combines multiple octave noise layers.
 *
 * This structure is used to generate surface noise for terrain generation by
 * combining multiple octave noise layers with different parameters.
 */
struct SurfaceNoise {
    double xzScale{};                        ///< Scale factor for the XZ plane.
    double yScale{};                         ///< Scale factor for the Y-axis.
    double xzFactor{};                       ///< Factor for the XZ plane.
    double yFactor{};                        ///< Factor for the Y-axis.
    OctaveNoise octaveMin;                   ///< Octave noise for minimum values.
    OctaveNoise octaveMax;                   ///< Octave noise for maximum values.
    OctaveNoise octaveMain;                  ///< Main octave noise.
    OctaveNoise octaveSurf;                  ///< Surface octave noise.
    OctaveNoise octaveDepth;                 ///< Depth octave noise.
    PerlinNoise oct[16 + 16 + 8 + 4 + 16]{}; ///< Array of Perlin noise layers.
};








/**
 * @brief Initializes a PerlinNoise structure with random values.
 *
 * This function sets up the Perlin noise parameters using a random number generator.
 *
 * @param noise Pointer to the PerlinNoise structure to initialize.
 * @param rng Reference to the random number generator.
 */
void perlinInit(PerlinNoise* noise, RNG& rng);

/**
 * @brief Samples Perlin noise at a specific 3D position.
 *
 * This function generates a Perlin noise value for the given coordinates and parameters.
 *
 * @param g Pointer to the Generator.
 * @param noise Pointer to the PerlinNoise structure.
 * @param x X-coordinate.
 * @param y Y-coordinate.
 * @param z Z-coordinate.
 * @param yAmplitude Amplitude for the Y-axis.
 * @param yMinimum Minimum value for the Y-axis.
 * @return The sampled Perlin noise value.
 */
double samplePerlin(const Generator* g, const PerlinNoise* noise,
                    double x, double y, double z,
                    double yAmplitude, double yMinimum);

/**
 * @brief Samples 2D Simplex noise using Perlin noise parameters.
 * Used by the End dimension.
 *
 * This function generates a 2D noise value based on the Perlin noise structure.
 *
 * @param noise Pointer to the PerlinNoise structure.
 * @param x X-coordinate.
 * @param y Y-coordinate.
 * @return The sampled 2D noise value.
 */
double sampleSimplex2D(const PerlinNoise* noise, double x, double y);

/// Octave noise

/**
 * @brief Initializes an OctaveNoise structure with random values.
 *
 * This function sets up the octave noise parameters using a random number generator.
 *
 * @param noise Pointer to the OctaveNoise structure to initialize.
 * @param rng Reference to the random number generator.
 * @param octaves Pointer to an array of PerlinNoise structures.
 * @param octaveMin Minimum octave index.
 * @param octaveCount Number of octaves to initialize.
 */
void octaveInit(OctaveNoise* noise, RNG& rng, PerlinNoise* octaves, int octaveMin, int octaveCount);

/**
 * @brief Samples octave noise at a specific 3D position.
 *
 * This function generates a noise value by combining multiple Perlin noise layers.
 *
 * @param g Pointer to the Generator.
 * @param noise Pointer to the OctaveNoise structure.
 * @param x X-coordinate.
 * @param y Y-coordinate.
 * @param z Z-coordinate.
 * @return The sampled octave noise value.
 */
MU double sampleOctave(const Generator* g, const OctaveNoise* noise,
                       double x, double y, double z);

/**
 * @brief Samples octave noise with additional amplitude and minimum Y-axis parameters.
 *
 * This function generates a noise value with additional control over the amplitude
 * and minimum Y-axis value.
 *
 * @param g Pointer to the Generator.
 * @param noise Pointer to the OctaveNoise structure.
 * @param x X-coordinate.
 * @param y Y-coordinate.
 * @param z Z-coordinate.
 * @param yAmp Amplitude for the Y-axis.
 * @param yMin Minimum value for the Y-axis.
 * @param yDefault Default value for the Y-axis.
 * @return The sampled octave noise value.
 */
double sampleOctaveAmp(const Generator* g, const OctaveNoise* noise,
                       double x, double y, double z, double yAmp,
                       double yMin, int yDefault);

/// Perlin Octaves

/**
 * @brief Initializes a SurfaceNoise structure for a specific dimension and world seed.
 *
 * This function sets up the surface noise parameters based on the dimension and world seed.
 *
 * @param sn Pointer to the SurfaceNoise structure to initialize.
 * @param dimension The dimension for which the surface noise is generated.
 * @param worldSeed The world seed used for noise generation.
 */
void initSurfaceNoise(SurfaceNoise* sn, lce::DIMENSION dimension, u64 worldSeed);