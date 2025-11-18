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

/**
 * @brief Main function to generate and visualize surface noise
 */
int main(int argc, char* argv[]) {
    // Configuration
    int64_t worldSeed = 0; // Default seed
    int imageWidth = 864;  // Width of output image
    int imageHeight = 864; // Height of output image
    const char* outputFile = "surface_noise.png";

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
    RNG rng;
    rng.setSeed(static_cast<u64>(worldSeed));
    rng.advance<10360>();

    NoiseGeneratorPerlin<4> surfaceNoise;
    surfaceNoise.setNoiseGeneratorPerlin(rng);

    std::cout << "Generating noise data...\n";

    // Allocate image buffer (grayscale)
    std::vector<uint8_t> imageData(imageWidth * imageHeight);

    // Track min/max for normalization
    double minNoise = std::numeric_limits<double>::max();
    double maxNoise = std::numeric_limits<double>::lowest();
    std::vector<double> noiseValues(imageWidth * imageHeight);

    // Process in 16x16 chunks
    int minChunkX = minWorldX >> 4;
    int minChunkZ = minWorldZ >> 4;
    int maxChunkX = (maxWorldX - 1) >> 4;
    int maxChunkZ = (maxWorldZ - 1) >> 4;

    std::array<double, 256> noiseRegion;

    for (int chunkZ = minChunkZ; chunkZ <= maxChunkZ; ++chunkZ) {
        for (int chunkX = minChunkX; chunkX <= maxChunkX; ++chunkX) {
            // Get the region for this chunk
            surfaceNoise.getRegion<16, 16, 0.0625, 0.0625, 0.5, 1.0>(lce::CONSOLE::XBOX360,
                noiseRegion, chunkX * 16, chunkZ * 16);

            // Copy noise values to the output buffer
            for (int localZ = 0; localZ < 16; ++localZ) {
                for (int localX = 0; localX < 16; ++localX) {
                    int worldX = chunkX * 16 + localX;
                    int worldZ = chunkZ * 16 + localZ;

                    // Check if within image bounds
                    if (worldX >= minWorldX && worldX < maxWorldX &&
                        worldZ >= minWorldZ && worldZ < maxWorldZ) {

                        double noiseValue = noiseRegion[localX + localZ * 16];

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

    for (int i = 0; i < imageWidth * imageHeight; ++i) {
        // Normalize to 0-1 range
        double normalized = (noiseValues[i] - minNoise) / range;

        // Convert to 0-255 grayscale
        imageData[i] = static_cast<uint8_t>(normalized * 255.0);
    }

    // Write the image
    std::cout << "Writing image to " << outputFile << "...\n";
    int result = stbi_write_png(outputFile, imageWidth, imageHeight, 1, imageData.data(), imageWidth);

    if (result) {
        std::cout << "Successfully saved image!\n";
    } else {
        std::cerr << "Failed to save image!\n";
        return 1;
    }

    return 0;
}
