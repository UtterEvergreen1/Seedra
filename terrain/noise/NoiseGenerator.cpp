#include "NoiseGenerator.hpp"


double NoiseGeneratorSimplex::getValue(c_double posX, c_double posZ) const {
    // Skew the input space to determine which simplex cell we're in
    const double skew = (posX + posZ) * SKEW_FACTOR;
    const int cellX = fastFloor(posX + skew);
    const int cellZ = fastFloor(posZ + skew);

    // Unskew the cell origin back to (x,z) space
    const double unskew = (cellX + cellZ) * UNSKEW_FACTOR;
    const double cellOriginX = cellX - unskew;
    const double cellOriginZ = cellZ - unskew;

    // Distances from the cell origin
    const double dx0 = posX - cellOriginX;
    const double dz0 = posZ - cellOriginZ;

    // Determine simplex corner ordering
    const int stepX = (dx0 > dz0) ? 1 : 0;
    const int stepZ = (dx0 > dz0) ? 0 : 1;

    // Corner offsets in (x,z)
    const double dx1 = dx0 - stepX + UNSKEW_FACTOR;
    const double dz1 = dz0 - stepZ + UNSKEW_FACTOR;
    const double dx2 = dx0 - 1.0 + 2.0 * UNSKEW_FACTOR;
    const double dz2 = dz0 - 1.0 + 2.0 * UNSKEW_FACTOR;

    // Hash the coordinates to get gradient indices (wrap 0..255)
    const int pX = cellX & 255;
    const int pZ = cellZ & 255;
    const int gi0 = permutations[pX + permutations[pZ]] % 12;
    const int gi1 = permutations[pX + stepX + permutations[pZ + stepZ]] % 12;
    const int gi2 = permutations[pX + 1 + permutations[pZ + 1]] % 12;

    // Contribution from each corner, with quartic attenuation
    double n0 = 0.0, n1 = 0.0, n2 = 0.0;

    double t0 = 0.5 - dx0 * dx0 - dz0 * dz0;
    if (t0 > 0.0) {
        // t0^4 faster than pow(t0, 4)
        t0 *= t0;
        t0 *= t0;
        n0 = t0 * dot(GRAD3[gi0], dx0, dz0);
    }

    double t1 = 0.5 - dx1 * dx1 - dz1 * dz1;
    if (t1 > 0.0) {
        t1 *= t1;
        t1 *= t1;
        n1 = t1 * dot(GRAD3[gi1], dx1, dz1);
    }

    double t2 = 0.5 - dx2 * dx2 - dz2 * dz2;
    if (t2 > 0.0) {
        t2 *= t2;
        t2 *= t2;
        n2 = t2 * dot(GRAD3[gi2], dx2, dz2);
    }

    // Combine contributions and scale the result
    return 70.0 * (n0 + n1 + n2);
}


void NoiseGeneratorSimplex::add(std::vector<double> &noiseValues,
                                double xOffset, double zOffset,
                                int width, int height,
                                double xScale, double zScale,
                                double noiseScale) {
    int writeIndex = 0;

    for (int zi = 0; zi < height; ++zi) {
        // Per-row z coordinate
        const double zCoord = (zOffset + zi) * zScale + this->y; // use per-instance offset

        for (int xi = 0; xi < width; ++xi) {
            const double xCoord = (xOffset + xi) * xScale + this->x; // use per-instance offset

            // Skew to find simplex cell
            const double skew = (xCoord + zCoord) * SKEW_FACTOR;
            const int cellX = fastFloor(xCoord + skew);
            const int cellZ = fastFloor(zCoord + skew);

            // Unskew back to (x,z)
            const double unskew = (cellX + cellZ) * UNSKEW_FACTOR;
            const double dx0 = xCoord - (cellX - unskew);
            const double dz0 = zCoord - (cellZ - unskew);

            // Determine simplex corner ordering
            const int stepX = (dx0 > dz0) ? 1 : 0;
            const int stepZ = (dx0 > dz0) ? 0 : 1;

            // Corner offsets in (x,z)
            const double dx1 = dx0 - stepX + UNSKEW_FACTOR;
            const double dz1 = dz0 - stepZ + UNSKEW_FACTOR;
            const double dx2 = dx0 - 1.0 + 2.0 * UNSKEW_FACTOR;
            const double dz2 = dz0 - 1.0 + 2.0 * UNSKEW_FACTOR;

            // Hash to pick gradient indices
            const int pX = cellX & 255;
            const int pZ = cellZ & 255;
            const int gi0 = permutations[pX + permutations[pZ]] % 12;
            const int gi1 = permutations[pX + stepX + permutations[pZ + stepZ]] % 12;
            const int gi2 = permutations[pX + 1 + permutations[pZ + 1]] % 12;

            // Corner contributions with quartic attenuation
            double n0 = 0.0, n1 = 0.0, n2 = 0.0;

            double t0 = 0.5 - dx0 * dx0 - dz0 * dz0;
            if (t0 > 0.0) {
                t0 *= t0;
                t0 *= t0;
                n0 = t0 * dot(GRAD3[gi0], dx0, dz0);
            }

            double t1 = 0.5 - dx1 * dx1 - dz1 * dz1;
            if (t1 > 0.0) {
                t1 *= t1;
                t1 *= t1;
                n1 = t1 * dot(GRAD3[gi1], dx1, dz1);
            }

            double t2 = 0.5 - dx2 * dx2 - dz2 * dz2;
            if (t2 > 0.0) {
                t2 *= t2;
                t2 *= t2;
                n2 = t2 * dot(GRAD3[gi2], dx2, dz2);
            }

            noiseValues[writeIndex++] += 70.0 * (n0 + n1 + n2) * noiseScale;
        }
    }
}


void NoiseGeneratorPerlin::setNoiseGeneratorPerlin(RNG &rng, c_i32 levelsIn) {
    levels = levelsIn;
    noiseLevels = std::vector<NoiseGeneratorSimplex>(levelsIn);

    for (int i = 0; i < levelsIn; ++i) {
        perlinInit(&noiseLevels[i], rng);
    }
}


void NoiseGeneratorImproved::populateNoiseArray(const Generator *g, std::vector<double> &noiseArray,
                                                double xOffset, double yOffset, double zOffset,
                                                int xSize, int ySize, int zSize,
                                                double xScale, double yScale, double zScale,
                                                double noiseScale) {
    if (ySize == 1) {
        // Special case for 2D noise generation.
        int permX0 = 0;
        int permRow0 = 0;
        int permX1 = 0;
        int permRow1 = 0;
        double nA = 0.0;
        double nB = 0.0;
        int writeIndex = 0;
        const double invScale = 1.0 / noiseScale;

        double xCoord = xOffset + x;
        for (int xi = 0; xi < xSize; ++xi) {
            int xInt = static_cast<int>(xCoord);
            if (xCoord < static_cast<double>(xInt)) { --xInt; }

            const int permX = xInt & 255;
            if (g->getConsole() != lce::CONSOLE::WIIU) {
                xCoord = xCoord - static_cast<double>(xInt);
            }
            const double fadeX = xCoord * xCoord * xCoord * (xCoord * (xCoord * 6.0 - 15.0) + 10.0);

            for (int zi = 0; zi < zSize; ++zi) {
                double zCoord = zOffset + static_cast<double>(zi) * zScale + z;
                int zInt = static_cast<int>(zCoord);
                if (zCoord < static_cast<double>(zInt)) { --zInt; }

                const int permZ = zInt & 255;
                if (g->getConsole() != lce::CONSOLE::WIIU) {
                    zCoord = zCoord - static_cast<double>(zInt);
                }
                const double fadeZ = zCoord * zCoord * zCoord * (zCoord * (zCoord * 6.0 - 15.0) + 10.0);

                permX0 = permutations[permX] + 0;
                permRow0 = permutations[permX0] + permZ;
                permX1 = permutations[permX + 1] + 0;
                permRow1 = permutations[permX1] + permZ;

                nA = MathHelper::lerp(fadeX,
                                      grad2(permutations[permRow0], xCoord, zCoord),
                                      grad(permutations[permRow1], xCoord - 1.0, 0.0, zCoord));
                nB = MathHelper::lerp(fadeX,
                                      grad(permutations[permRow0 + 1], xCoord, 0.0, zCoord - 1.0),
                                      grad(permutations[permRow1 + 1], xCoord - 1.0, 0.0, zCoord - 1.0));
                const double n = MathHelper::lerp(fadeZ, nA, nB);
                noiseArray[writeIndex++] += n * invScale;
            }
            xCoord += xInt + xScale;
        }
    } else {
        // General case for 3D noise generation.
        int writeIndex = 0;
        const double invScale = 1.0 / noiseScale;
        int lastYInt = -1;
        int permBase0 = 0;
        int permXZ00 = 0;
        int permXZ01 = 0;
        int permBase1 = 0;
        int permXZ10 = 0;
        int permXZ12 = 0;
        double n000 = 0.0;
        double n010 = 0.0;
        double n001 = 0.0;
        double n011 = 0.0;

        for (int xi = 0; xi < xSize; ++xi) {
            double xCoord = xOffset + static_cast<double>(xi) * xScale + x;
            int xInt = static_cast<int>(xCoord);
            if (xCoord < static_cast<double>(xInt)) { --xInt; }

            const int permX = xInt & 255;
            xCoord = xCoord - static_cast<double>(xInt);
            const double fadeX = xCoord * xCoord * xCoord * (xCoord * (xCoord * 6.0 - 15.0) + 10.0);

            for (int zi = 0; zi < zSize; ++zi) {
                double zCoord = zOffset + static_cast<double>(zi) * zScale + z;
                int zInt = static_cast<int>(zCoord);
                if (zCoord < static_cast<double>(zInt)) { --zInt; }

                const int permZ = zInt & 255;
                zCoord = zCoord - static_cast<double>(zInt);
                const double fadeZ = zCoord * zCoord * zCoord * (zCoord * (zCoord * 6.0 - 15.0) + 10.0);

                for (int yi = 0; yi < ySize; yi++) {
                    double yCoord = yOffset + static_cast<double>(yi) * yScale + y;
                    int yInt = static_cast<int>(yCoord);
                    if (yCoord < static_cast<double>(yInt)) { --yInt; }

                    const int permY = yInt & 255;
                    yCoord = yCoord - static_cast<double>(yInt);
                    const double fadeY = yCoord * yCoord * yCoord * (yCoord * (yCoord * 6.0 - 15.0) + 10.0);

                    if (yi == 0 || permY != lastYInt) {
                        lastYInt = permY;
                        permBase0 = permutations[permX] + permY;
                        permXZ00 = permutations[permBase0] + permZ;
                        permXZ01 = permutations[permBase0 + 1] + permZ;
                        permBase1 = permutations[permX + 1] + permY;
                        permXZ10 = permutations[permBase1] + permZ;
                        permXZ12 = permutations[permBase1 + 1] + permZ; // same as permXZ11 + 1 in original code
                        n000 = MathHelper::lerp(fadeX,
                                                grad(permutations[permXZ00], xCoord, yCoord, zCoord),
                                                grad(permutations[permXZ10], xCoord - 1.0, yCoord, zCoord));
                        n010 = MathHelper::lerp(fadeX,
                                                grad(permutations[permXZ01], xCoord, yCoord - 1.0, zCoord),
                                                grad(permutations[permXZ12], xCoord - 1.0, yCoord - 1.0, zCoord));
                        n001 = MathHelper::lerp(fadeX,
                                                grad(permutations[permXZ00 + 1], xCoord, yCoord, zCoord - 1.0),
                                                grad(permutations[permXZ10 + 1], xCoord - 1.0, yCoord, zCoord - 1.0));
                        n011 = MathHelper::lerp(fadeX,
                                                grad(permutations[permXZ01 + 1], xCoord, yCoord - 1.0, zCoord - 1.0),
                                                grad(permutations[permXZ12 + 1], xCoord - 1.0, yCoord - 1.0,
                                                     zCoord - 1.0));
                    }

                    const double n0 = MathHelper::lerp(fadeY, n000, n010);
                    const double n1 = MathHelper::lerp(fadeY, n001, n011);
                    const double n = MathHelper::lerp(fadeZ, n0, n1);
                    noiseArray[writeIndex++] += n * invScale;
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


void NoiseGeneratorOctaves::genNoiseOctaves(const Generator *g, std::vector<double> &noiseArray,
                                            c_i32 xOffset, c_i32 yOffset, c_i32 zOffset,
                                            c_i32 xSize, c_i32 ySize, c_i32 zSize,
                                            c_double xScale, c_double yScale, c_double zScale) {
    // Ensure the noise array is properly sized and initialized
    if EXPECT_TRUE(noiseArray.empty()) {
        noiseArray.resize(static_cast<size_t>(xSize) * static_cast<size_t>(ySize) * static_cast<size_t>(zSize), 0.0);
    } else {
        std::fill(noiseArray.begin(), noiseArray.end(), 0.0);
    }

    double amplitude = 1.0;

    // Generate noise for each octave
    for (int octave = 0; octave < octaves; ++octave) {
        const double sx = xOffset * amplitude * xScale;
        const double sy = yOffset * amplitude * yScale;
        const double sz = zOffset * amplitude * zScale;

        i64 flooredX = lfloor(sx);
        i64 flooredZ = lfloor(sz);

        double xFrac = sx - static_cast<double>(flooredX);
        double zFrac = sz - static_cast<double>(flooredZ);

        flooredX %= 16777216LL;
        flooredZ %= 16777216LL;

        xFrac += static_cast<double>(flooredX);
        zFrac += static_cast<double>(flooredZ);

        generatorCollection[octave].populateNoiseArray(g, noiseArray,
                                                       xFrac, sy, zFrac,
                                                       xSize, ySize, zSize,
                                                       xScale * amplitude,
                                                       yScale * amplitude,
                                                       zScale * amplitude,
                                                       amplitude);

        amplitude *= 0.5;
    }
}
