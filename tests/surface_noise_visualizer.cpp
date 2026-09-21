/**
 * @file surface_noise_visualizer.cpp
 * @brief Standalone program to visualize the surface noise from a Minecraft world seed
 *
 * This program generates a black and white image showing the surface noise pattern
 * for a given world seed. The noise is used in terrain generation.
 */


#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

#include "terrain/noise/NoiseGen.hpp"
#include "common/rng.hpp"

// STB image write for PNG output
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lce/include/stb_image_write.hpp"

#define getPerm(idx) s_perm[idx]
#define setPerm(idx, val) s_perm[idx] = val

double fade(double t)
{
    double t2 = t * t;
    double t3 = t2 * t;
    return t3 * t * ((t * 6.0f) -15.0f) + 10.0f;
}

MU ND static double grad(c_int hash, c_double x, c_double y, c_double z) {
    static constexpr double GRAD_X[16] = { 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0 };
    static constexpr double GRAD_Y[16] = { 1.0, 1.0, -1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0 };
    static constexpr double GRAD_Z[16] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 0.0, 1.0, 0.0, -1.0 };

    c_int i = hash & 15;
    return GRAD_X[i] * x + GRAD_Y[i] * y + GRAD_Z[i] * z;
}

MU ND static double grad_2d(c_int hash, c_double x, c_double z) {
    static constexpr double GRAD_2X[16] = { 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0 };
    static constexpr double GRAD_2Z[16] = { 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 0.0, 1.0, 0.0, -1.0 };

    c_int i = hash & 15;
    return GRAD_2X[i] * x + GRAD_2Z[i] * z;
}

MU static int fast_floor(c_double value) {
    return value > 0.0 ? static_cast<int>(value) : static_cast<int>(value) - 1;
}

double indexedGrad(int idx, double a, double b, double c) {
    switch (idx & 0xF)
    {
        case 0:  return  a + b;
        case 1:  return -a + b;
        case 2:  return  a - b;
        case 3:  return -a - b;
        case 4:  return  a + c;
        case 5:  return -a + c;
        case 6:  return  a - c;
        case 7:  return -a - c;
        case 8:  return  b + c;
        case 9:  return -b + c;
        case 10: return  b - c;
        case 11: return -b - c;
        case 12: return  a + b;
        case 13: return -b + c;
        case 14: return -a + b;
        case 15: return -b - c;
        default: return  0;

#if __GNUC__
            __builtin_unreachable();
#endif
            return 0;
    }
}

double simplexGrad(int idx, double x, double y)
{
    double con = 0.5 - x * x - y * y;
    if (con < 0) return 0;
    con *= con;
    return con * con * indexedGrad(idx, x, y, 0);
}

struct NoiseGen {
    uint8_t s_perm[256];
    double xCoord;
    double yCoord;
    double zCoord;

    NoiseGen(RNG& rng) {
        this->xCoord = rng.nextDouble() * 256.0;
        this->yCoord = rng.nextDouble() * 256.0;
        this->zCoord = rng.nextDouble() * 256.0;

        // B. Generate the 256 permutations for THIS octave into Shared Memory
#pragma unroll
        for (int i = 0; i < 256; i++) {
            setPerm(i, i);
        }

        for (int i = 0; i < 256; i++) {
            int j = rng.nextInt(256 - i) + i;
            uint8_t tmp = getPerm(i);
            setPerm(i, getPerm(j));
            setPerm(j, tmp);
        }
    }

    double sample_simplex_2d(double x, double y)
    {
        constexpr double SQRT_3 = 1.7320508075688772; // sqrt(3.0);
        constexpr double SKEW = 0.5 * (SQRT_3 - 1.0);
        constexpr double UNSKEW = (3.0 - SQRT_3) / 6.0;

        // Skew input space to determine which simplex cell we're in
        double skewOffset = (x + y) * SKEW;
        int skewedX = static_cast<int>(floor(x + skewOffset));
        int skewedY = static_cast<int>(floor(y + skewOffset));

        // Unskew the cell origin back to (x,y) space
        double unskewOffset = (skewedX + skewedY) * UNSKEW;
        double corner0_x = x - (skewedX - unskewOffset);
        double corner0_y = y - (skewedY - unskewOffset);

        // Determine which simplex we're in (upper or lower triangle)
        int middleCornerOffsetX = (corner0_x > corner0_y);
        int middleCornerOffsetY = !middleCornerOffsetX;

        // Offsets for middle and last corner in unskewed (x,y) coords
        double corner1_x = corner0_x - middleCornerOffsetX + UNSKEW;
        double corner1_y = corner0_y - middleCornerOffsetY + UNSKEW;
        double corner2_x = corner0_x - 1.0 + 2.0 * UNSKEW;
        double corner2_y = corner0_y - 1.0 + 2.0 * UNSKEW;

        // Hash coordinates to get gradient indices
        int gi0 = getPerm(0xFF & (skewedY));
        int gi1 = getPerm(0xFF & (skewedY + middleCornerOffsetY));
        int gi2 = getPerm(0xFF & (skewedY + 1));
        gi0 = getPerm(0xFF & (gi0 + skewedX));
        gi1 = getPerm(0xFF & (gi1 + skewedX + middleCornerOffsetX));
        gi2 = getPerm(0xFF & (gi2 + skewedX + 1));

        // Calculate noise contributions from each corner
        double noiseSum = 0;
        noiseSum += simplexGrad(gi0 % 12, corner0_x, corner0_y);
        noiseSum += simplexGrad(gi1 % 12, corner1_x, corner1_y);
        noiseSum += simplexGrad(gi2 % 12, corner2_x, corner2_y);


        return 70.0 * noiseSum;
    }

    double sample_perlin_2d(double x, double z)
    {

        int X = (int)(x);
        int Z = (int)(z);

        double fx = x - (double)X;
        double fz = z - (double)Z;

        X &= 255;
        Z &= 255;

        double u = fade(fx);
        double w = fade(fz);

        int A = getPerm(X);
        int B = getPerm((X + 1) & 255);

        int AA = getPerm(A) + Z;
        int BB = getPerm(B) + Z;

        int hash00 = getPerm(AA & 255);
        int hash01 = getPerm((AA + 1) & 255);
        int hash10 = getPerm(BB & 255);
        int hash11 = getPerm((BB + 1) & 255);

        double g00 = grad_2d(hash00, fx, fz);
        double g10 = grad_2d(hash10, fx - 1.0, fz);
        double g01 = grad_2d(hash01, fx, fz - 1.0);
        double g11 = grad_2d(hash11, fx - 1.0, fz - 1.0);

        double lerp_X0 = lerp(u, g00, g10);
        double lerp_X1 = lerp(u, g01, g11);

        return lerp(w, lerp_X0, lerp_X1);
    }

    double sample_perlin_3d(
        double x, double z,
        double y_base, double y_scale, int y_iter)
    {
        int cubeX = fast_floor(x);
        int cubeZ = fast_floor(z);

        double px = x - (double)cubeX;
        double pz = z - (double)cubeZ;

        cubeX &= 255;
        cubeZ &= 255;

        double fadeX = fade(px);
        double fadeZ = fade(pz);

        double pxm1 = px - 1.0;
        double pzm1 = pz - 1.0;

        // -----------------------------------------------------
        // THE Y CACHING LOGIC
        // -----------------------------------------------------
        // 1. Calculate the true Y position and its cell
        double py_final = (double)y_iter * y_scale + y_base;
        int Y_final = fast_floor(py_final);
        int Y_cell_final = Y_final & 255;
        double py_final_frac = py_final - (double)Y_final;

        int cache_iter = y_iter;

        // 2. Walk backwards to find when this Y cell was first entered
        #pragma unroll 4
        for (int curr_y = y_iter - 1; curr_y >= 0; curr_y--) {
            double py_check = (double)curr_y * y_scale + y_base;
            int Y_check = fast_floor(py_check);
            if ((Y_check & 255) != Y_cell_final) break;
            cache_iter = curr_y;
        }

        // 3. Calculate the fractional Y from the cache origin
        double py_cache = (double)cache_iter * y_scale + y_base;
        int Y_cache = fast_floor(py_cache);
        double py_cache_frac = py_cache - (double)Y_cache;
        double py_cache_frac_m1 = py_cache_frac - 1.0;

        // 4. Fade MUST use the REAL final fraction, not the cached one!
        double fadeY = fade(py_final_frac);
        // -----------------------------------------------------

        // Permutation lookups
        int A  = getPerm(cubeX) + Y_cell_final;
        int B  = getPerm((cubeX + 1) & 255) + Y_cell_final;

        int perm_A = getPerm(A & 255);
        int perm_A1 = getPerm((A + 1) & 255);
        int perm_B = getPerm(B & 255);
        int perm_B1 = getPerm((B + 1) & 255);

        int AA = perm_A + cubeZ;
        int AB = perm_A1 + cubeZ;
        int BA = perm_B + cubeZ;
        int BB = perm_B1 + cubeZ;

        // Preload all 8 gradient hash values
        int h000 = getPerm(AA & 255);
        int h100 = getPerm(BA & 255);
        int h010 = getPerm(AB & 255);
        int h110 = getPerm(BB & 255);
        int h001 = getPerm((AA + 1) & 255);
        int h101 = getPerm((BA + 1) & 255);
        int h011 = getPerm((AB + 1) & 255);
        int h111 = getPerm((BB + 1) & 255);

        // Compute all 8 gradients using the CACHED py fractions
        double g000 = grad(h000, px, py_cache_frac, pz);
        double g100 = grad(h100, pxm1, py_cache_frac, pz);
        double g010 = grad(h010, px, py_cache_frac_m1, pz);
        double g110 = grad(h110, pxm1, py_cache_frac_m1, pz);
        double g001 = grad(h001, px, py_cache_frac, pzm1);
        double g101 = grad(h101, pxm1, py_cache_frac, pzm1);
        double g011 = grad(h011, px, py_cache_frac_m1, pzm1);
        double g111 = grad(h111, pxm1, py_cache_frac_m1, pzm1);

        // Trilinear interpolation
        // Interpolate along X axis (4 edges)
        double lerp_y0 = lerp(fadeX, g000, g100);
        double lerp_y1 = lerp(fadeX, g010, g110);
        double lerp_y2 = lerp(fadeX, g001, g101);
        double lerp_y3 = lerp(fadeX, g011, g111);

        // Interpolate along Y axis (2 edges) using the REAL fadeY
        double lerp_z0 = lerp(fadeY, lerp_y0, lerp_y1);
        double lerp_z1 = lerp(fadeY, lerp_y2, lerp_y3);

        // Interpolate along Z axis (final result)
        return lerp(fadeZ, lerp_z0, lerp_z1);
    }
};

struct NoiseGenCollection {
    std::vector<NoiseGen> collection;
    int octaves;

    NoiseGenCollection(RNG& rng, int numOctaves) {
        octaves = numOctaves;
        for (int i = 0; i < octaves; i++) {
            collection.emplace_back(rng);
        }
    }

    void generate_climate_map(
        double base_scale,
        double lacunarity,
        int blockX,
        int blockZ,
        double* grid)
    {
#pragma unroll
        for (int i = 0; i < 256; i++) {
            grid[i] = 0.0;
        }

        double current_amplitude = 1.0;

        for (int oct = 0; oct < this->octaves; oct++) {
            NoiseGen& gen = this->collection[oct];
            for (int x = 0; x < 16; x++) {
                for (int z = 0; z < 16; z++) {
                    int grid_idx = x * 16 + z;

                    // Calculate absolute world block coordinates for this node
                    // Note: Beta 1.6.6 spacing is grid step 3 with a +1 offset
                    double noise_x =  (blockX + x) * base_scale + gen.xCoord;
                    double noise_z = (blockZ + z) * base_scale + gen.yCoord;

                    double noise_val = gen.sample_simplex_2d(noise_x, noise_z);

                    // Accumulate to the grid
                    grid[grid_idx] += noise_val * (0.55 / current_amplitude);
                }
            }

            base_scale *= lacunarity;
            // base_scale /= 2.0;
            current_amplitude /= 2.0;
            // current_amplitude *= lacunarity;
        }
    }

    void generate_perlin_2d(
        double base_scale,
        int blockX,
        int blockZ,
        double* grid)
    {
#pragma unroll
        for (int i = 0; i < 25; i++) {
            grid[i] = 0.0;
        }

        double current_amplitude = 1.0;
        constexpr static int lacunarity = 2;

        // 2. THE OCTAVE-FIRST LOOP
        for (int oct = 0; oct < octaves; oct++) {
            NoiseGen& gen = this->collection[oct];
            for (int x = 0; x < 5; x++) {
                for (int z = 0; z < 5; z++) {
                    int grid_idx = x * 5 + z;

                    // Calculate absolute world block coordinates for this node
                    // Note: Beta 1.6.6 spacing is grid step 3 with a +1 offset
                    double noise_x =  (blockX + x) * base_scale + gen.xCoord;
                    double noise_z = (blockZ + z) * base_scale + gen.zCoord;

                    double noise_val = gen.sample_perlin_2d(noise_x, noise_z);

                    // Accumulate to the grid
                    grid[grid_idx] += noise_val * current_amplitude;
                }
            }

            base_scale /= lacunarity;
            current_amplitude *= 2.0;
        }
    }

    void generate_perlin_3d(
        double base_scale_xz,
        double base_scale_y,
        int blockX,
        int target_y_segment,
        int blockZ,
        double* grid)
    {
#pragma unroll
        for (int i = 0; i < 25; i++) {
            grid[i] = 0.0;
        }

        double current_amplitude = 1.0;
        constexpr static int lacunarity = 2;

        // 2. THE OCTAVE-FIRST LOOP
        for (int oct = 0; oct < octaves; oct++) {
            NoiseGen& gen = this->collection[oct];

            for (int x = 0; x < 5; x++) {
                for (int z = 0; z < 5; z++) {
                    int grid_idx = x * 5 + z;

                    double noise_x = (blockX + x) * base_scale_xz + gen.xCoord;
                    double noise_z = (blockZ + z) * base_scale_xz + gen.zCoord;

                    // Pass the y_base, y_scale, and the y_segment directly to the function
                    double noise_val = gen.sample_perlin_3d(
                        noise_x,
                        noise_z,
                        gen.yCoord,   // y_base
                        base_scale_y,       // y_scale
                        target_y_segment    // y_iter
                    );

                    // Accumulate to the grid
                    grid[grid_idx] += noise_val * current_amplitude;
                }
            }

            base_scale_xz /= lacunarity;
            base_scale_y /= lacunarity;
            current_amplitude *= 2.0;
        }
    }
};

/**
 * @brief Main function to generate and visualize surface noise
 */
int main(int argc, char* argv[]) {
    // Configuration
    int64_t worldSeed = 0; // Default seed
    int imageWidth = 864;  // Width of output image
    int imageHeight = 864; // Height of output image
    const char* outputFile = "biome_noise";

    // Parse command line arguments
    if (argc > 1) {
        worldSeed = std::stoll(argv[1]);
    }
    if (argc > 2) {
        imageWidth = std::stoi(argv[2]);
    }
    if (argc > 3) {
        imageHeight = std::stoi(argv[3]);
    }
    if (argc > 4) {
        outputFile = argv[4];
    }

    int minWorldX = -imageWidth / 2;
    int minWorldZ = -imageHeight / 2;
    int maxWorldX = minWorldX + imageWidth;
    int maxWorldZ = minWorldZ + imageHeight;

    std::cout << "Surface Noise Visualizer\n";
    std::cout << "========================\n";
    std::cout << "World Seed: " << worldSeed << "\n";
    std::cout << "Image Size: " << imageWidth << "x" << imageHeight << "\n";
    std::cout << "Output File: " << outputFile << "\n\n";

    // Initialize the noise generator
    RNG rngTemp(9871LL);
    RNG rngHumid(39811LL);
    RNG rngDetail(543321LL);
    NoiseGenCollection tempNoise(rngTemp, 4);
    NoiseGenCollection humidNoise(rngHumid, 4);
    NoiseGenCollection detailNoise(rngDetail, 2);

    // NoiseGeneratorPerlin<4> surfaceNoise;
    // surfaceNoise.setNoiseGeneratorPerlin(rng);
    // NoiseGeneratorPerlin<1> TEMPERATURE_NOISE;
    /*NoiseGeneratorPerlin<4> tempNoise(rngTemp);
    NoiseGeneratorPerlin<4> humidNoise(rngHumid);
    NoiseGeneratorPerlin<2> detailNoise(rngDetail);*/
    // TEMPERATURE_NOISE.setNoiseGeneratorPerlin(rng);

    std::cout << "Generating noise data...\n";

    // Allocate image buffer (grayscale)
    std::vector<uint8_t> imageData(imageWidth * imageHeight);

    // Track min/max for normalization
    std::vector<double> noiseValues(imageWidth * imageHeight);

    // Process in 16x16 chunks
    int minChunkX = minWorldX >> 4;
    int minChunkZ = minWorldZ >> 4;
    int maxChunkX = (maxWorldX - 1) >> 4;
    int maxChunkZ = (maxWorldZ - 1) >> 4;

    std::array<double, 256> noiseRegion;
    // std::array<double, 25> noiseRegion;

    for (int i = 0; i < 3; i++) {
        double minNoise = std::numeric_limits<double>::max();
        double maxNoise = std::numeric_limits<double>::lowest();

        for (int chunkZ = minChunkZ; chunkZ <= maxChunkZ; ++chunkZ) {
            for (int chunkX = minChunkX; chunkX <= maxChunkX; ++chunkX) {
                // Get the region for this chunk
                if (i == 0) {
                    tempNoise.generate_climate_map(0.025 / 1.5, 0.25, chunkX * 16, chunkZ * 16, noiseRegion.data());
                } else if (i == 1) {
                    humidNoise.generate_climate_map(0.05 / 1.5, 1.0 / 3.0, chunkX * 16, chunkZ * 16, noiseRegion.data());
                } else if (i == 2) {
                    humidNoise.generate_climate_map(0.25 / 1.5, 0.5882352941176471, chunkX * 16, chunkZ * 16, noiseRegion.data());
                }


                /*if (i == 0) {
                    tempNoise.getRegion<16, 16, 0.025 / 1.5, 0.025 / 1.5, 0.5, 0.25>(lce::CONSOLE::XBOX360,
                        noiseRegion, chunkX * 16, chunkZ * 16);
                } else if (i == 1) {
                    humidNoise.getRegion<16, 16, 0.05 / 1.5, 0.05 / 1.5, 0.5, 1.0 / 3.0>(lce::CONSOLE::XBOX360,
                        noiseRegion, chunkX * 16, chunkZ * 16);
                } else if (i == 2) {
                    detailNoise.getRegion<16, 16, 0.25 / 1.5, 0.25 / 1.5, 0.5, 0.5882352941176471>(lce::CONSOLE::XBOX360,
                        noiseRegion, chunkX * 16, chunkZ * 16);
                }*/

                // Copy noise values to the output buffer
                // for (int localZ = 0; localZ < 5; ++localZ) {
                for (int localZ = 0; localZ < 16; ++localZ) {
                    // for (int localX = 0; localX < 5; ++localX) {
                    for (int localX = 0; localX < 16; ++localX) {
                        // int worldX = chunkX * 16 + (localX * 3 + 1);
                        int worldX = chunkX * 16 + localX;
                        // int worldZ = chunkZ * 16 + (localZ * 3 + 1);
                        int worldZ = chunkZ * 16 + localZ;

                        // Check if within image bounds
                        if (worldX >= minWorldX && worldX < maxWorldX &&
                            worldZ >= minWorldZ && worldZ < maxWorldZ) {
                            double noiseValue = noiseRegion[localX * 16 + localZ];
                            // double noiseValue = noiseRegion[localX * 5 + localZ];

                            int idx = (worldX - minWorldX) + (worldZ - minWorldZ) * imageWidth;
                            noiseValues[idx] = noiseValue;

                            // Update min/max
                            minNoise = std::min(minNoise, noiseValue);
                            maxNoise = std::max(maxNoise, noiseValue);
                        }
                    }
                }
            }

            // Progress indicator
            if ((chunkZ - minChunkZ + 1) % 10 == 0 || chunkZ == maxChunkZ) {
                std::cout << "Progress: " << (chunkZ - minChunkZ + 1) << "/" << (maxChunkZ - minChunkZ + 1) << " chunks\r" << std::flush;
            }
        }

        std::cout << "\n";
        std::cout << "Noise Range: [" << minNoise << ", " << maxNoise << "]" << " Midpoint: " << (minNoise + maxNoise) / 2.0 << "\n";

        // Normalize and convert to grayscale
        std::cout << "Converting to image...\n";
        double range = maxNoise - minNoise;

        if (range < 1e-10) {
            std::cerr << "Warning: Noise range is too small, image may be uniform\n";
            range = 1.0;
        }

        for (int j = 0; j < imageWidth * imageHeight; ++j) {
            // Normalize to 0-1 range
            double normalized = (noiseValues[j] - minNoise) / range;

            // Convert to 0-255 grayscale
            imageData[j] = static_cast<uint8_t>(normalized * 255.0);
        }

        // Write the image
        std::string finalFileName = outputFile + std::to_string(i) + ".png";
        std::cout << "Writing image to " << finalFileName << "...\n";
        int result = stbi_write_png(finalFileName.c_str(), imageWidth, imageHeight, 1, imageData.data(), imageWidth);

        if (result) {
            std::cout << "Successfully saved image!\n";
        } else {
            std::cerr << "Failed to save image!\n";
            return 1;
        }
    }

    return 0;
}
