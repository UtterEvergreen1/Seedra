#include "NoiseGenerator.hpp"


double NoiseGeneratorSimplex::getValue(c_double posX, c_double posZ) const {
    // Skewing and unskewing factors
    static constexpr double SKEW_FACTOR = 0.5 * (SQRT_3 - 1.0);
    static constexpr double UNSKEW_FACTOR = (3.0 - SQRT_3) / 6.0;

    // Skew the input space
    double skew = (posX + posZ) * SKEW_FACTOR;
    int skewedX = fastFloor(posX + skew);
    int skewedZ = fastFloor(posZ + skew);

    // Unskew the cell origin
    double unskew = (skewedX + skewedZ) * UNSKEW_FACTOR;
    double cellOriginX = skewedX - unskew;
    double cellOriginZ = skewedZ - unskew;

    // Distances from the cell origin
    double xDist = posX - cellOriginX;
    double zDist = posZ - cellOriginZ;

    // Determine simplex corner offsets
    int offsetX = (xDist > zDist) ? 1 : 0;
    int offsetZ = (xDist > zDist) ? 0 : 1;

    // Calculate the positions of the simplex corners
    double x1 = xDist - offsetX + UNSKEW_FACTOR;
    double z1 = zDist - offsetZ + UNSKEW_FACTOR;
    double x2 = xDist - 1.0 + 2.0 * UNSKEW_FACTOR;
    double z2 = zDist - 1.0 + 2.0 * UNSKEW_FACTOR;

    // Hash the coordinates to get gradient indices
    int permX = skewedX & 255;
    int permZ = skewedZ & 255;
    int gradIndex0 = permutations[permX + permutations[permZ]] % 12;
    int gradIndex1 = permutations[permX + offsetX + permutations[permZ + offsetZ]] % 12;
    int gradIndex2 = permutations[permX + 1 + permutations[permZ + 1]] % 12;

    // Calculate contributions from each corner
    double contribution0 = 0.5 - xDist * xDist - zDist * zDist;
    double noise0 = (contribution0 < 0) ? 0 : pow(contribution0, 4) * dot(GRAD3[gradIndex0], xDist, zDist);

    double contribution1 = 0.5 - x1 * x1 - z1 * z1;
    double noise1 = (contribution1 < 0) ? 0 : pow(contribution1, 4) * dot(GRAD3[gradIndex1], x1, z1);

    double contribution2 = 0.5 - x2 * x2 - z2 * z2;
    double noise2 = (contribution2 < 0) ? 0 : pow(contribution2, 4) * dot(GRAD3[gradIndex2], x2, z2);

    // Combine contributions and scale the result
    return 70.0 * (noise0 + noise1 + noise2);
}


void NoiseGeneratorSimplex::add(std::vector<double>& noiseValues, double xOffset, double zOffset,
                                int width, int height, double xScale, double zScale, double noiseScale) {
    static constexpr double F2 = 0.5 * (SQRT_3 - 1.0);
    static constexpr double G2 = (3.0 - SQRT_3) / 6.0;

    int index = 0;

    for (int z = 0; z < height; ++z) {
        double zCoord = (zOffset + z) * zScale + y;

        for (int x = 0; x < width; ++x) {
            double xCoord = (xOffset + x) * xScale + x;
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
            int gradIndex0 = permutations[permX + permutations[permZ]] % 12;
            int gradIndex1 = permutations[permX + offsetX + permutations[permZ + offsetZ]] % 12;
            int gradIndex2 = permutations[permX + 1 + permutations[permZ + 1]] % 12;

            double contribution0 = 0.5 - xUnskewed * xUnskewed - zUnskewed * zUnskewed;
            double noise0 = (contribution0 < 0)
                            ? 0
                            : pow(contribution0, 4) * dot(GRAD3[gradIndex0], xUnskewed, zUnskewed);

            double contribution1 = 0.5 - x1 * x1 - z1 * z1;
            double noise1 = (contribution1 < 0) ? 0 : pow(contribution1, 4) * dot(GRAD3[gradIndex1], x1, z1);

            double contribution2 = 0.5 - x2 * x2 - z2 * z2;
            double noise2 = (contribution2 < 0) ? 0 : pow(contribution2, 4) * dot(GRAD3[gradIndex2], x2, z2);

            noiseValues[index++] += 70.0 * (noise0 + noise1 + noise2) * noiseScale;
        }
    }
}


void NoiseGeneratorPerlin::setNoiseGeneratorPerlin(RNG& rng, c_i32 levelsIn) {
    levels = levelsIn;
    noiseLevels = std::vector<NoiseGeneratorSimplex>(levelsIn);

    for (int i = 0; i < levelsIn; ++i) {
        perlinInit(&noiseLevels[i], rng);
    }
}


void NoiseGeneratorImproved::populateNoiseArray(const Generator* g, std::vector<double>& noiseArray,
                                                double xOffset, double yOffset, double zOffset,
                                                int xSize, int ySize, int zSize,
                                                double xScale, double yScale, double zScale,
                                                double noiseScale) {
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

        double d17 = xOffset + x;
        for (int j2 = 0; j2 < xSize; ++j2) {
            int i6 = (int) d17;

            if (d17 < (double) i6) { --i6; }

            int k2 = i6 & 255;
            if (g->getConsole() != lce::CONSOLE::WIIU) {
                d17 = d17 - (double) i6;
            }
            double d18 = d17 * d17 * d17 * (d17 * (d17 * 6.0 - 15.0) + 10.0);

            for (int j6 = 0; j6 < zSize; ++j6) {
                double d19 = zOffset + (double) j6 * zScale + z;
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
            double d5 = xOffset + (double) l2 * xScale + x;
            int i3 = (int) d5;

            if (d5 < (double) i3) { --i3; }

            int j3 = i3 & 255;
            d5 = d5 - (double) i3;
            double d6 = d5 * d5 * d5 * (d5 * (d5 * 6.0 - 15.0) + 10.0);

            for (int k3 = 0; k3 < zSize; ++k3) {
                double d7 = zOffset + (double) k3 * zScale + z;
                int l3 = (int) d7;

                if (d7 < (double) l3) { --l3; }

                int i4 = l3 & 255;
                d7 = d7 - (double) l3;
                double d8 = d7 * d7 * d7 * (d7 * (d7 * 6.0 - 15.0) + 10.0);

                for (int j4 = 0; j4 < ySize; j4++) {
                    double d9 = yOffset + (double) j4 * yScale + y;
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
}


/*
 * @brief Computes the floor of a double value.
 * @param value The input value.
 * @return The floor of the input value as a 64-bit integer.
 */
static i64 lfloor(c_double value) {
    c_auto i = (i64) value;
    return value < (double) i ? i - 1L : i;
}


void NoiseGeneratorOctaves::genNoiseOctaves(const Generator* g, std::vector<double>& noiseArray,
                                            c_i32 xOffset, c_i32 yOffset, c_i32 zOffset,
                                            c_i32 xSize, c_i32 ySize, c_i32 zSize,
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

        scaledXOffset -= (double)flooredX;
        scaledZOffset -= (double)flooredZ;

        flooredX %= 16777216LL;
        flooredZ %= 16777216LL;

        scaledXOffset += (double)flooredX;
        scaledZOffset += (double)flooredZ;

        generatorCollection[i].populateNoiseArray(g, noiseArray,
                                                  scaledXOffset, scaledYOffset, scaledZOffset,
                                                  xSize, ySize, zSize,
                                                  xScale * amplitude,
                                                  yScale * amplitude,
                                                  zScale * amplitude,
                                                  amplitude);

        amplitude /= 2.0;
    }
}
