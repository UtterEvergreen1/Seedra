#include "NoiseGen.hpp"

#include "terrain/generator.hpp"

using namespace noise_values;

/**
 * @brief Generates simplex noise for a given 2D position.
 * @param posX The X-coordinate of the position.
 * @param posZ The Z-coordinate of the position.
 * @return The noise value at the given position.
 */
double NoiseGeneratorSimplex::getValue(c_double posX, c_double posZ) const {
    // Skew the input space to determine which simplex cell we're in
    const double skew = (posX + posZ) * NOISE_SIMPLEX_SKEW_FACTOR;
    const int cellX = fastFloor(posX + skew);
    const int cellZ = fastFloor(posZ + skew);

    // Unskew the cell origin back to (x,z) space
    const double unskew = (cellX + cellZ) * NOISE_SIMPLEX_UNSKEW_FACTOR;
    const double cellOriginX = cellX - unskew;
    const double cellOriginZ = cellZ - unskew;

    // Distances from the cell origin
    const double dx0 = posX - cellOriginX;
    const double dz0 = posZ - cellOriginZ;

    // Determine simplex corner ordering
    const int stepX = (dx0 > dz0) ? 1 : 0;
    const int stepZ = (dx0 > dz0) ? 0 : 1;

    // Corner offsets in (x,z)
    const double dx1 = dx0 - stepX + NOISE_SIMPLEX_UNSKEW_FACTOR;
    const double dz1 = dz0 - stepZ + NOISE_SIMPLEX_UNSKEW_FACTOR;
    const double dx2 = dx0 - 1.0 + 2.0 * NOISE_SIMPLEX_UNSKEW_FACTOR;
    const double dz2 = dz0 - 1.0 + 2.0 * NOISE_SIMPLEX_UNSKEW_FACTOR;

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
        n0 = t0 * dot(NOISE_SIMPLEX_GRAD3[gi0], dx0, dz0);
    }

    double t1 = 0.5 - dx1 * dx1 - dz1 * dz1;
    if (t1 > 0.0) {
        t1 *= t1;
        t1 *= t1;
        n1 = t1 * dot(NOISE_SIMPLEX_GRAD3[gi1], dx1, dz1);
    }

    double t2 = 0.5 - dx2 * dx2 - dz2 * dz2;
    if (t2 > 0.0) {
        t2 *= t2;
        t2 *= t2;
        n2 = t2 * dot(NOISE_SIMPLEX_GRAD3[gi2], dx2, dz2);
    }

    // Combine contributions and scale the result
    return 70.0 * (n0 + n1 + n2);
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
/*
void NoiseGeneratorSimplex::add(std::vector<double> &noiseValues,
                                double xOffset, double zOffset,
                                int width, int height,
                                double xScale, double zScale,
                                double noiseScale) const {
    // int writeIndex = 0;
    double* __restrict out = noiseValues.data();
    const double K = 70.0 * noiseScale;

    for (int zi = 0; zi < height; ++zi) {
        // Per-row z coordinate
        const double zCoord = (zOffset + zi) * zScale + this->y; // use per-instance offset

        for (int xi = 0; xi < width; ++xi) {
            const double xCoord = (xOffset + xi) * xScale + this->x; // use per-instance offset

            // Skew to find simplex cell
            const double skew = (xCoord + zCoord) * NOISE_SIMPLEX_SKEW_FACTOR;
            const int cellX = fastFloor(xCoord + skew);
            const int cellZ = fastFloor(zCoord + skew);

            // Unskew back to (x,z)
            const double unskew = (cellX + cellZ) * NOISE_SIMPLEX_UNSKEW_FACTOR;
            const double dx0 = xCoord - (cellX - unskew);
            const double dz0 = zCoord - (cellZ - unskew);

            // Determine simplex corner ordering
            const int stepX = (dx0 > dz0) ? 1 : 0;
            const int stepZ = (dx0 > dz0) ? 0 : 1;

            // Corner offsets in (x,z)
            const double dx1 = dx0 - stepX + NOISE_SIMPLEX_UNSKEW_FACTOR;
            const double dz1 = dz0 - stepZ + NOISE_SIMPLEX_UNSKEW_FACTOR;
            const double dx2 = dx0 - 1.0 + 2.0 * NOISE_SIMPLEX_UNSKEW_FACTOR;
            const double dz2 = dz0 - 1.0 + 2.0 * NOISE_SIMPLEX_UNSKEW_FACTOR;

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
                n0 = t0 * dot(NOISE_SIMPLEX_GRAD3[gi0], dx0, dz0);
            }

            double t1 = 0.5 - dx1 * dx1 - dz1 * dz1;
            if (t1 > 0.0) {
                t1 *= t1;
                t1 *= t1;
                n1 = t1 * dot(NOISE_SIMPLEX_GRAD3[gi1], dx1, dz1);
            }

            double t2 = 0.5 - dx2 * dx2 - dz2 * dz2;
            if (t2 > 0.0) {
                t2 *= t2;
                t2 *= t2;
                n2 = t2 * dot(NOISE_SIMPLEX_GRAD3[gi2], dx2, dz2);
            }

            *out++ += K * (n0 + n1 + n2);
        }
    }
}
*/

template<int Width, int Height>
void NoiseGeneratorSimplex::add(std::array<double, Width * Height> &noiseValues,
             c_double xOffset, c_double zOffset,
             c_double xScale, c_double zScale, c_double noiseScale) const {
    // int writeIndex = 0;
    double* __restrict out = noiseValues.data();
    const double K = 70.0 * noiseScale;

    for (int zi = 0; zi < Height; ++zi) {
        // Per-row z coordinate
        const double zCoord = (zOffset + zi) * zScale + this->y; // use per-instance offset

        for (int xi = 0; xi < Width; ++xi) {
            const double xCoord = (xOffset + xi) * xScale + this->x; // use per-instance offset

            // Skew to find simplex cell
            const double skew = (xCoord + zCoord) * NOISE_SIMPLEX_SKEW_FACTOR;
            const int cellX = fastFloor(xCoord + skew);
            const int cellZ = fastFloor(zCoord + skew);

            // Unskew back to (x,z)
            const double unskew = (cellX + cellZ) * NOISE_SIMPLEX_UNSKEW_FACTOR;
            const double dx0 = xCoord - (cellX - unskew);
            const double dz0 = zCoord - (cellZ - unskew);

            // Determine simplex corner ordering
            const int stepX = (dx0 > dz0) ? 1 : 0;
            const int stepZ = (dx0 > dz0) ? 0 : 1;

            // Corner offsets in (x,z)
            const double dx1 = dx0 - stepX + NOISE_SIMPLEX_UNSKEW_FACTOR;
            const double dz1 = dz0 - stepZ + NOISE_SIMPLEX_UNSKEW_FACTOR;
            const double dx2 = dx0 - 1.0 + 2.0 * NOISE_SIMPLEX_UNSKEW_FACTOR;
            const double dz2 = dz0 - 1.0 + 2.0 * NOISE_SIMPLEX_UNSKEW_FACTOR;

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
                n0 = t0 * dot(NOISE_SIMPLEX_GRAD3[gi0], dx0, dz0);
            }

            double t1 = 0.5 - dx1 * dx1 - dz1 * dz1;
            if (t1 > 0.0) {
                t1 *= t1;
                t1 *= t1;
                n1 = t1 * dot(NOISE_SIMPLEX_GRAD3[gi1], dx1, dz1);
            }

            double t2 = 0.5 - dx2 * dx2 - dz2 * dz2;
            if (t2 > 0.0) {
                t2 *= t2;
                t2 *= t2;
                n2 = t2 * dot(NOISE_SIMPLEX_GRAD3[gi2], dx2, dz2);
            }

            *out++ += K * (n0 + n1 + n2);
        }
    }
}


/*
void NoiseGeneratorImproved::populateNoiseArray(const Generator *g, std::vector<double> &noiseArray,
                                                double xOffset, double yOffset, double zOffset,
                                                int xSize, int ySize, int zSize,
                                                double xScale, double yScale, double zScale,
                                                double noiseScale) const {
    const double invScale = 1.0 / noiseScale;
    double* __restrict out = noiseArray.data();
    const u8* __restrict P = permutations;

    if (ySize == 1) {
        // Special case for 2D noise generation.
        const bool isWiiU = (g->getConsole() == lce::CONSOLE::WIIU);

        double xCoord = xOffset + x;
        for (int xi = 0; xi < xSize; ++xi) {
            int xInt = static_cast<int>(xCoord);
            if (xCoord < static_cast<double>(xInt)) { --xInt; }

            const int permX = xInt & 255;
            if (!isWiiU) { xCoord = xCoord - static_cast<double>(xInt); }
            const double fadeX = xCoord*xCoord*xCoord * (xCoord*(xCoord*6.0 - 15.0) + 10.0);

            for (int zi = 0; zi < zSize; ++zi) {
                double zCoord = zOffset + static_cast<double>(zi) * zScale + z;
                int zInt = static_cast<int>(zCoord);
                if (zCoord < static_cast<double>(zInt)) { --zInt; }

                const int permZ = zInt & 255;
                if (!isWiiU) { zCoord = zCoord - static_cast<double>(zInt); }
                const double fadeZ = zCoord*zCoord*zCoord * (zCoord * (zCoord*6.0 - 15.0) + 10.0);

                int permX0 = P[permX] + 0;
                int permRow0 = P[permX0] + permZ;
                int permX1 = P[permX + 1] + 0;
                int permRow1 = P[permX1] + permZ;

                double nA = MathHelper::lerp(fadeX,
                                       grad2(P[permRow0], xCoord, zCoord),
                                       grad(P[permRow1], xCoord - 1.0, 0.0, zCoord));
                double nB = MathHelper::lerp(fadeX,
                                        grad(P[permRow0 + 1], xCoord, 0.0, zCoord - 1.0),
                                        grad(P[permRow1 + 1], xCoord - 1.0, 0.0, zCoord - 1.0));
                *out++ += MathHelper::lerp(fadeZ, nA, nB) * invScale;
            }
            xCoord += xInt + xScale;
        }
    } else {
        // General case for 3D noise generation.
        int writeIndex = 0;
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
            const double fadeX = xCoord*xCoord*xCoord * (xCoord*(xCoord*6.0 - 15.0) + 10.0);

            for (int zi = 0; zi < zSize; ++zi) {
                double zCoord = zOffset + static_cast<double>(zi) * zScale + z;
                int zInt = static_cast<int>(zCoord);
                if (zCoord < static_cast<double>(zInt)) { --zInt; }

                const int permZ = zInt & 255;
                zCoord = zCoord - static_cast<double>(zInt);
                const double fadeZ = zCoord*zCoord*zCoord * (zCoord * (zCoord*6.0 - 15.0) + 10.0);

                for (int yi = 0; yi < ySize; yi++) {
                    double yCoord = yOffset + static_cast<double>(yi) * yScale + y;
                    int yInt = static_cast<int>(yCoord);
                    if (yCoord < static_cast<double>(yInt)) { --yInt; }

                    const int permY = yInt & 255;
                    yCoord = yCoord - static_cast<double>(yInt);
                    const double fadeY = yCoord*yCoord*yCoord * (yCoord*(yCoord*6.0 - 15.0) + 10.0);

                    if (yi == 0 || permY != lastYInt) {
                        lastYInt = permY;
                        permBase0 = P[permX] + permY;
                        permXZ00 = P[permBase0] + permZ;
                        permXZ01 = P[permBase0 + 1] + permZ;
                        permBase1 = P[permX + 1] + permY;
                        permXZ10 = P[permBase1] + permZ;
                        permXZ12 = P[permBase1 + 1] + permZ; // same as permXZ11 + 1 in original code
                        n000 = MathHelper::lerp(fadeX,
                                                grad(P[permXZ00], xCoord, yCoord, zCoord),
                                                grad(P[permXZ10], xCoord - 1.0, yCoord, zCoord));
                        n010 = MathHelper::lerp(fadeX,
                                                grad(P[permXZ01], xCoord, yCoord - 1.0, zCoord),
                                                grad(P[permXZ12], xCoord - 1.0, yCoord - 1.0, zCoord));
                        n001 = MathHelper::lerp(fadeX,
                                                grad(P[permXZ00 + 1], xCoord, yCoord, zCoord - 1.0),
                                                grad(P[permXZ10 + 1], xCoord - 1.0, yCoord, zCoord - 1.0));
                        n011 = MathHelper::lerp(fadeX,
                                                grad(P[permXZ01 + 1], xCoord, yCoord - 1.0, zCoord - 1.0),
                                                grad(P[permXZ12 + 1], xCoord - 1.0, yCoord - 1.0,
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
*/
/**
 * @brief Populates a noise array with generated noise values.
 * @param g The generator object used for console-specific behavior.
 * @param noiseArray The array to store the generated noise values.
 * @param xOffset The X-offset for noise generation.
 * @param yOffset The Y-offset for noise generation.
 * @param zOffset The Z-offset for noise generation.
 * @tparam XSize The size of the noise array in the X-dimension.
 * @tparam YSize The size of the noise array in the Y-dimension.
 * @tparam ZSize The size of the noise array in the Z-dimension.
 * @param xScale The scale factor for the X-dimension.
 * @param yScale The scale factor for the Y-dimension.
 * @param zScale The scale factor for the Z-dimension.
 * @param noiseScale The overall scale factor for the noise.
 * @return The populated noise array.
 */
template<int XSize, int YSize, int ZSize>
    void NoiseGeneratorImproved::populateNoiseArrayImpl(
        const Generator *g, std::array<double, XSize * YSize * ZSize> &noiseArray,
        double xOffset, double yOffset, double zOffset,
        double xScale, double yScale, double zScale, double noiseScale) const {
    const double invScale = 1.0 / noiseScale;
    double* __restrict out = noiseArray.data();
    const u8* __restrict P = permutations;

    auto fade = [](c_double t) noexcept {
        return t*t*t * (t*(t*6.0 - 15.0) + 10.0);
    };

    // 2D Path (ySize == 1) noise generation.
    if constexpr (YSize == 1) {
        const bool isWiiU = (g->getConsole() == lce::CONSOLE::WIIU);

        // Precompute Z values used for every xi
        int pZ[ZSize];
        double tz[ZSize];
        double fz[ZSize];
        {
            for (int zi = 0; zi < ZSize; ++zi) {
                double zCoord = zOffset + static_cast<double>(zi) * zScale + z;
                int zInt = static_cast<int>(zCoord);
                if (zCoord < static_cast<double>(zInt)) { --zInt; }
                const int permZ = zInt & 255;
                if (!isWiiU) { zCoord = zCoord - static_cast<double>(zInt); }
                pZ[zi] = permZ;
                tz[zi] = zCoord;
                fz[zi] = fade(zCoord);
            }
        }

        double xCoord = xOffset + x;
        for (int xi = 0; xi < XSize; ++xi) {
            int xInt = static_cast<int>(xCoord);
            if (xCoord < static_cast<double>(xInt)) { --xInt; }

            const int permX = xInt & 255;
            if (!isWiiU) { xCoord = xCoord - static_cast<double>(xInt); }
            const double fadeX = fade(xCoord);

            const int permX0 = P[permX] + 0;
            const int permX1 = P[permX + 1] + 0;

            for (int zi = 0; zi < ZSize; ++zi) {
                const int    permZ = pZ[zi];
                const double zFrac = tz[zi];
                const double fZ    = fz[zi];

                const int permRow0 = P[permX0] + permZ;
                const int permRow1 = P[permX1] + permZ;

                const double nA = MathHelper::lerp(fadeX,
                                                   grad2(P[permRow0], xCoord, zFrac),
                                                   grad (P[permRow1], xCoord - 1.0, 0.0, zFrac));

                const double nB = MathHelper::lerp(fadeX,
                                                   grad (P[permRow0 + 1], xCoord, 0.0, zFrac - 1.0),
                                                   grad (P[permRow1 + 1], xCoord - 1.0, 0.0, zFrac - 1.0));

                *out++ += MathHelper::lerp(fZ, nA, nB) * invScale;
            }

            // Preserve exact stepping semantics
            xCoord += xInt + xScale;
        }
        return;
    }



    // General case for 3D noise generation.


    // Precompute Z terms (independent of xi/yi)
    int pZ[ZSize];
    double tz[ZSize];
    double fz[ZSize];
    {
        for (int zi = 0; zi < ZSize; ++zi) {
            double zCoord = zOffset + static_cast<double>(zi) * zScale + z;
            int zInt = static_cast<int>(zCoord);
            if (zCoord < static_cast<double>(zInt)) { --zInt; }
            const int permZ = zInt & 255;
            zCoord = zCoord - static_cast<double>(zInt);
            pZ[zi] = permZ;
            tz[zi] = zCoord;
            fz[zi] = fade(zCoord);
        }
    }

    // Precompute Y terms (independent of xi/zi)
    int pY[YSize];
    double ty[YSize];
    double fy[YSize];
    {
        for (int yi = 0; yi < YSize; ++yi) {
            double yCoord = yOffset + static_cast<double>(yi) * yScale + y;
            int yInt = static_cast<int>(yCoord);
            if (yCoord < static_cast<double>(yInt)) { --yInt; }
            const int permY = yInt & 255;
            yCoord = yCoord - static_cast<double>(yInt);
            pY[yi] = permY;
            ty[yi] = yCoord;
            fy[yi] = fade(yCoord);
        }
    }

    int writeIndex = 0;
    for (int xi = 0; xi < XSize; ++xi) {
        double xCoord = xOffset + static_cast<double>(xi) * xScale + x;
        int xInt = static_cast<int>(xCoord);
        if (xCoord < static_cast<double>(xInt)) { --xInt; }

        const int permX = xInt & 255;
        xCoord = xCoord - static_cast<double>(xInt);
        const double fadeX = fade(xCoord);

        for (int zi = 0; zi < ZSize; ++zi) {
            const int    permZ = pZ[zi];
            const double zFrac = tz[zi];
            const double fZ    = fz[zi];

            int lastYInt = -1;
            int permBase0 = 0;
            int permXZ00  = 0;
            int permXZ01  = 0;
            int permBase1 = 0;
            int permXZ10  = 0;
            int permXZ12  = 0;

            double n000 = 0.0;
            double n010 = 0.0;
            double n001 = 0.0;
            double n011 = 0.0;

            for (int yi = 0; yi < YSize; ++yi) {
                const int    permY = pY[yi];
                const double yFrac = ty[yi];
                const double fY    = fy[yi];

                if (yi == 0 || permY != lastYInt) {
                    lastYInt = permY;
                    permBase0 = P[permX] + permY;
                    permXZ00  = P[permBase0]     + permZ;
                    permXZ01  = P[permBase0 + 1] + permZ;
                    permBase1 = P[permX + 1] + permY;
                    permXZ10  = P[permBase1]     + permZ;
                    permXZ12  = P[permBase1 + 1] + permZ; // same as permXZ11 + 1

                    n000 = MathHelper::lerp(fadeX,
                                            grad(P[permXZ00],     xCoord,     yFrac,     zFrac),
                                            grad(P[permXZ10],     xCoord - 1.0, yFrac,     zFrac));

                    n010 = MathHelper::lerp(fadeX,
                                            grad(P[permXZ01],     xCoord,     yFrac - 1.0, zFrac),
                                            grad(P[permXZ12],     xCoord - 1.0, yFrac - 1.0, zFrac));

                    n001 = MathHelper::lerp(fadeX,
                                            grad(P[permXZ00 + 1], xCoord,     yFrac,     zFrac - 1.0),
                                            grad(P[permXZ10 + 1], xCoord - 1.0, yFrac,     zFrac - 1.0));

                    n011 = MathHelper::lerp(fadeX,
                                            grad(P[permXZ01 + 1], xCoord,     yFrac - 1.0, zFrac - 1.0),
                                            grad(P[permXZ12 + 1], xCoord - 1.0, yFrac - 1.0, zFrac - 1.0));
                }

                const double n0 = MathHelper::lerp(fY, n000, n010);
                const double n1 = MathHelper::lerp(fY, n001, n011);
                const double n  = MathHelper::lerp(fZ, n0, n1);
                noiseArray[writeIndex++] += n * invScale;
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


/*
void NoiseGeneratorOctaves::genNoiseOctaves(
    const Generator *g, std::vector<double> &noiseArray,
    c_int xOffset, c_int yOffset, c_int zOffset,
    c_int xSize, c_int ySize, c_int zSize,
    c_double xScale, c_double yScale, c_double zScale) const {

    // Dispatch to templated version based on runtime sizes
    if (xSize == 5 && ySize == 1 && zSize == 5) {
        genNoiseOctavesImpl<5, 1, 5>(g, noiseArray, xOffset, yOffset, zOffset, xScale, yScale, zScale);
    } else if (xSize == 5 && ySize == 33 && zSize == 5) {
        genNoiseOctavesImpl<5, 33, 5>(g, noiseArray, xOffset, yOffset, zOffset, xScale, yScale, zScale);
    } else {
        // Fallback for unusual sizes (shouldn't happen in practice)
        fprintf(stderr, "Warning: Unsupported noise size %dx%dx%d\n", xSize, ySize, zSize);
        // Could implement a slow fallback here if needed
    }
}
*/

/*
void NoiseGeneratorOctaves::genNoiseOctaves(const Generator *g, std::vector<double> &noiseArray,
                                            c_i32 xOffset, c_i32 yOffset, c_i32 zOffset,
                                            c_i32 xSize, c_i32 ySize, c_i32 zSize,
                                            c_double xScale, c_double yScale, c_double zScale) const {
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
*/

/**
 * @brief Generates noise values for a 3D region using multiple octaves.
 * @param g The generator object used for console-specific behavior.
 * @param noiseArray The array to store the generated noise values.
 * @param xOffset The X-offset for noise generation.
 * @param yOffset The Y-offset for noise generation.
 * @param zOffset The Z-offset for noise generation.
 * @tparam XSize The size of the region in the X-dimension.
 * @tparam YSize The size of the region in the Y-dimension.
 * @tparam ZSize The size of the region in the Z-dimension.
 * @tparam XScale The scale factor for the X-dimension.
 * @tparam YScale The scale factor for the Y-dimension.
 * @tparam ZScale The scale factor for the Z-dimension.
 * @return A vector containing the generated noise values.
 */
template<int Octaves>
template<typename Container, int XSize, int YSize, int ZSize, double XScale, double YScale, double ZScale>
void NoiseGeneratorOctaves<Octaves>::genNoiseOctavesImpl(
    const Generator *g, std::array<Container, XSize * YSize * ZSize> &noiseArray,
    c_int xOffset, c_int yOffset, c_int zOffset) const {

    std::memset(noiseArray.data(), 0, sizeof(noiseArray));

    double amplitude = 1.0;

    for (int octave = 0; octave < Octaves; ++octave) {
        const double sx = xOffset * amplitude * XScale;
        const double sy = yOffset * amplitude * YScale;
        const double sz = zOffset * amplitude * ZScale;

        i64 flooredX = lfloor(sx);
        i64 flooredZ = lfloor(sz);

        double xFrac = sx - static_cast<double>(flooredX);
        double zFrac = sz - static_cast<double>(flooredZ);

        flooredX %= 16777216LL;
        flooredZ %= 16777216LL;

        xFrac += static_cast<double>(flooredX);
        zFrac += static_cast<double>(flooredZ);

        generatorCollection[octave].template populateNoiseArrayImpl<XSize, YSize, ZSize>(
            g, noiseArray,
            xFrac, sy, zFrac,
            XScale * amplitude,
            YScale * amplitude,
            ZScale * amplitude,
            amplitude);

        amplitude *= 0.5;
    }
}

template class NoiseGeneratorPerlin<1>;
template class NoiseGeneratorPerlin<4>;

template void NoiseGeneratorSimplex::add<16, 16>(std::array<double, 256>&, double, double, double, double, double) const;

template void NoiseGeneratorImproved::populateNoiseArrayImpl<5, 1, 5>(const Generator *, std::array<double, 25> &, double, double, double, double, double, double, double) const;
template void NoiseGeneratorImproved::populateNoiseArrayImpl<5, 33, 5>(const Generator *, std::array<double, 825> &, double, double, double, double, double, double, double) const;

template class NoiseGeneratorOctaves<8>;
template class NoiseGeneratorOctaves<16>;

template void NoiseGeneratorOctaves<16>::genNoiseOctavesImpl<double, 5,  1, 5, 200.0,   1.0,      200.0  >(const Generator *, std::array<double, 25> &, int, int, int) const;
template void NoiseGeneratorOctaves<8>::genNoiseOctavesImpl<double, 5, 33, 5, 8.55515, 4.277575, 8.55515>(const Generator *, std::array<double, 825> &, int, int, int) const;
template void NoiseGeneratorOctaves<16>::genNoiseOctavesImpl<double, 5, 33, 5, 684.412, 684.412,  684.412>(const Generator *, std::array<double, 825> &, int, int, int) const;
