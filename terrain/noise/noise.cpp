
#include <cmath>
#include <cstdio>
#include <cstring>

#include "noise.hpp"
#include "common/MathHelper.hpp"
#include "common/rng.hpp"
#include "terrain/generator.hpp"

static constexpr double SQRT_3 = 1.7320508075688772; // sqrt(3.0);

double maintainPrecision(c_double x) { return x - floor(x / 33554432.0 + 0.5) * 33554432.0; }

ATTR(hot, const, always_inline, artificial)
inline static double indexedLerp(c_int idx, c_double a, c_double b, c_double c) {
    switch (idx & 0xF) {
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
    }
#if __GNUC__
    __builtin_unreachable();
#endif
    return 0;
}

void perlinInit(PerlinNoise* noise, RNG& rng) {

    noise->x = rng.nextDouble() * 256.0;
    noise->y = rng.nextDouble() * 256.0;
    noise->z = rng.nextDouble() * 256.0;
    noise->amplitude = 1.0;
    noise->lacunarity = 1.0;


    static constexpr std::array<u8, 256> oneTo255Array = []() constexpr {
        std::array<u8, 256> arr = {};
        for (int i = 0; i < 256; ++i) {
            arr[i] = i;
        }
        return arr;
    }();

    // Initialize the first 256 elements of the permutation array.
    memcpy(noise->permutations, oneTo255Array.data(), 256 * sizeof(u8));

    // Shuffle the permutation array and duplicate it.
    for (int i = 0; i < 256; ++i) {
        int j = rng.nextInt(256 - i) + i;
        u8 k = noise->permutations[i];
        noise->permutations[i] = noise->permutations[j];
        noise->permutations[j] = k;
        noise->permutations[i + 256] = noise->permutations[i];
    }
}


double samplePerlin(const Generator* g, const PerlinNoise* noise,
                    double x, double y, double z,
                    c_double yAmplitude, c_double yMinimum) {
    // apply random offset to make this noise unique
    x += noise->x;
    y += noise->y;
    z += noise->z;

    // find the unit cube that contains the point (floor)
    int cubeX = static_cast<int>(x) - static_cast<int>(x < 0);
    int cubeY = static_cast<int>(y) - static_cast<int>(y < 0);
    int cubeZ = static_cast<int>(z) - static_cast<int>(z < 0);

    // get position within the unit cube [0, 1)
    if (g->getConsole() != lce::CONSOLE::WIIU) {
        x -= cubeX;
        y -= cubeY;
        z -= cubeZ;
    }

    // compute smoothstep/fade curves for smooth interpolation
    c_double fadeX = x * x * x * (x * (x * 6.0 - 15.0) + 10.0);
    c_double fadeY = y * y * y * (y * (y * 6.0 - 15.0) + 10.0);
    c_double fadeZ = z * z * z * (z * (z * 6.0 - 15.0) + 10.0);

    // Optional: Make noise repeat in Y direction
    if (yAmplitude != 0.0) {
        c_double yClamp = yMinimum < y ? yMinimum : y;
        y -= floor(yClamp / yAmplitude) * yAmplitude;
    }

    // wrap cube coordinates to [0, 255] for permutation table lookup
    cubeX &= 0xFF;
    cubeY &= 0xFF;
    cubeZ &= 0xFF;

    // Hash coordinates of the 8 cube corners
    // These create unique indices into the permutation table
    c_int hashXY = noise->permutations[cubeX] + cubeY;
    c_int hashXYZ_000 = noise->permutations[hashXY] + cubeZ;
    c_int hashXYZ_010 = noise->permutations[hashXY + 1] + cubeZ;
    c_int hashX1Y = noise->permutations[cubeX + 1] + cubeY;
    c_int hashXYZ_100 = noise->permutations[hashX1Y] + cubeZ;
    c_int hashXYZ_110 = noise->permutations[hashX1Y + 1] + cubeZ;

    double gradCorner000   = indexedLerp(noise->permutations[hashXYZ_000], x, y, z);
    c_double gradCorner100 = indexedLerp(noise->permutations[hashXYZ_100], x - 1, y, z);
    double gradCorner010   = indexedLerp(noise->permutations[hashXYZ_010], x, y - 1, z);
    c_double gradCorner110 = indexedLerp(noise->permutations[hashXYZ_110], x - 1, y - 1, z);
    double gradCorner001   = indexedLerp(noise->permutations[hashXYZ_000 + 1], x, y, z - 1);
    c_double gradCorner101 = indexedLerp(noise->permutations[hashXYZ_100 + 1], x - 1, y, z - 1);
    double gradCorner011   = indexedLerp(noise->permutations[hashXYZ_010 + 1], x, y - 1, z - 1);
    c_double gradCorner111 = indexedLerp(noise->permutations[hashXYZ_110 + 1], x - 1, y - 1, z - 1);

    // Trilinear interpolation
    // Step 1: Interpolate along X axis (4 edges)
    gradCorner000 = MathHelper::lerp(fadeX, gradCorner000, gradCorner100);
    gradCorner010 = MathHelper::lerp(fadeX, gradCorner010, gradCorner110);
    gradCorner001 = MathHelper::lerp(fadeX, gradCorner001, gradCorner101);
    gradCorner011 = MathHelper::lerp(fadeX, gradCorner011, gradCorner111);

    // Step 2: Interpolate along Y axis (2 edges)
    gradCorner000 = MathHelper::lerp(fadeY, gradCorner000, gradCorner010);
    gradCorner001 = MathHelper::lerp(fadeY, gradCorner001, gradCorner011);

    // Step 3: Interpolate along Z axis (final result)
    return MathHelper::lerp(fadeZ, gradCorner000, gradCorner001);
}


MU static double simplexGrad(c_int idx, c_double x, c_double y, c_double z, c_double d) {
    double con = d - x * x - y * y - z * z;
    if (con < 0) return 0;
    con *= con;
    return con * con * indexedLerp(idx, x, y, z);
}


static double simplexGrad_HARDCODED(c_int idx, c_double x, c_double y) {
    double con = 0.5 - x * x - y * y;
    if (con < 0) return 0;
    con *= con;
    return con * con * indexedLerp(idx, x, y, 0);
}


double sampleSimplex2D(const PerlinNoise* noise, c_double x, c_double y) {
    static constexpr double SKEW = 0.5 * (SQRT_3 - 1.0);
    static constexpr double UNSKEW = (3.0 - SQRT_3) / 6.0;

    // Skew input space to determine which simplex cell we're in
    c_double skewOffset = (x + y) * SKEW;
    c_int skewedX = static_cast<int>(floor(x + skewOffset));
    c_int skewedY = static_cast<int>(floor(y + skewOffset));

    // Unskew the cell origin back to (x,y) space
    c_double unskewOffset = (skewedX + skewedY) * UNSKEW;
    c_double corner0_x = x - (skewedX - unskewOffset);
    c_double corner0_y = y - (skewedY - unskewOffset);

    // Determine which simplex we're in (upper or lower triangle)
    c_int middleCornerOffsetX = (corner0_x > corner0_y);
    c_int middleCornerOffsetY = !middleCornerOffsetX;

    // Offsets for middle and last corner in unskewed (x,y) coords
    c_double corner1_x = corner0_x - middleCornerOffsetX + UNSKEW;
    c_double corner1_y = corner0_y - middleCornerOffsetY + UNSKEW;
    c_double corner2_x = corner0_x - 1.0 + 2.0 * UNSKEW;
    c_double corner2_y = corner0_y - 1.0 + 2.0 * UNSKEW;

    // Hash coordinates to get gradient indices
    int gi0 = noise->permutations[0xFF & (skewedY)];
    int gi1 = noise->permutations[0xFF & (skewedY + middleCornerOffsetY)];
    int gi2 = noise->permutations[0xFF & (skewedY + 1)];
    gi0 = noise->permutations[0xFF & (gi0 + skewedX)];
    gi1 = noise->permutations[0xFF & (gi1 + skewedX + middleCornerOffsetX)];
    gi2 = noise->permutations[0xFF & (gi2 + skewedX + 1)];

    // Calculate noise contributions from each corner
    double noiseSum = 0;
    noiseSum += simplexGrad_HARDCODED(gi0 % 12, corner0_x, corner0_y); // , 0.0, 0.5
    noiseSum += simplexGrad_HARDCODED(gi1 % 12, corner1_x, corner1_y); // , 0.0, 0.5
    noiseSum += simplexGrad_HARDCODED(gi2 % 12, corner2_x, corner2_y); // , 0.0, 0.5


    return 70.0 * noiseSum;
}


void octaveInit(OctaveNoise* noise, RNG& rng, PerlinNoise* octaves, c_int octaveMin, c_int octaveCount) {
    int currentIndex;
    c_int octaveMax = octaveMin + octaveCount - 1;

    // Calculate initial persistence (normalization factor)
    double persistence = 1.0 / (static_cast<double>(1LL << octaveCount) - 1.0);

    // Calculate initial lacunarity (frequency multiplier)
    double lacunarity = pow(2.0, octaveMax);

    // Validate octave range (must end at or below octave 0)
    if EXPECT_FALSE(octaveCount < 1 || octaveMax > 0) {
        printf("octavePerlinInit(): unsupported octave range\n");
        return;
    }

    // Special case: if range includes octave 0, initialize it first
    if (octaveMax == 0) {
        perlinInit(&octaves[0], rng);
        octaves[0].amplitude = persistence;
        octaves[0].lacunarity = lacunarity;
        persistence *= 2.0; // Next octave has double amplitude
        lacunarity *= 0.5; // Next octave has half frequency
        currentIndex = 1;
    } else {
        // Skip RNG calls for "missing" octaves to maintain determinism
        // Each perlinInit makes 262 RNG calls
        rng.skipNextN(-octaveMax * 262);
        currentIndex = 0;
    }

    // Initialize remaining octaves
    for (; currentIndex < octaveCount; currentIndex++) {
        perlinInit(&octaves[currentIndex], rng);
        octaves[currentIndex].amplitude = persistence;
        octaves[currentIndex].lacunarity = lacunarity;
        persistence *= 2.0; // Amplitude increases for lower-frequency octaves
        lacunarity *= 0.5; // Frequency decreases for lower-frequency octaves
    }

    noise->octaves = octaves;
    noise->octcnt = octaveCount;
}


MU double sampleOctave(const Generator* g, const OctaveNoise* noise,
                       c_double x, c_double y, c_double z) {
    double noiseSum = 0;
    for (int i = 0; i < noise->octcnt; i++) {
        const PerlinNoise* octave = noise->octaves + i;
        c_double frequencyMultiplier = octave->lacunarity;
        c_double scaledX = maintainPrecision(x * frequencyMultiplier);
        c_double scaledY = maintainPrecision(y * frequencyMultiplier);
        c_double scaledZ = maintainPrecision(z * frequencyMultiplier);
        c_double octaveValue = samplePerlin(g, octave, scaledX, scaledY, scaledZ, 0, 0);
        noiseSum += octave->amplitude * octaveValue;
    }
    return noiseSum;
}


void initSurfaceNoise(SurfaceNoise* sn, const lce::DIMENSION dimension, c_u64 worldSeed) {
    RNG rng;
    rng.setSeed(worldSeed);
    octaveInit(&sn->octaveMin, rng, sn->oct + 0, -15, 16);
    octaveInit(&sn->octaveMax, rng, sn->oct + 16, -15, 16);
    octaveInit(&sn->octaveMain, rng, sn->oct + 32, -7, 8);
    if (dimension == lce::DIMENSION::END) {
        sn->xzScale = 2.0;
        sn->yScale = 1.0;
        sn->xzFactor = 80;
        sn->yFactor = 160;
    } else { // DIM_OVERWORLD
        octaveInit(&sn->octaveSurf, rng, sn->oct + 40, -3, 4);
        rng.skipNextN(262 * 10);
        octaveInit(&sn->octaveDepth, rng, sn->oct + 44, -15, 16);
        sn->xzScale = 0.9999999814507745;
        sn->yScale = 0.9999999814507745;
        sn->xzFactor = 80;
        sn->yFactor = 160;
    }
}


double sampleOctaveAmp(const Generator* g, const OctaveNoise* noise, c_double x, c_double y, c_double z, c_double yAmp,
                       c_double yMin, c_int yDefault) {
    double v = 0;
    for (int i = 0; i < noise->octcnt; i++) {
        const PerlinNoise* p = noise->octaves + i;
        c_double lf = p->lacunarity;
        c_double ax = maintainPrecision(x * lf);
        c_double ay = yDefault ? -p->y : maintainPrecision(y * lf);
        c_double az = maintainPrecision(z * lf);
        c_double pv = samplePerlin(g, p, ax, ay, az, yAmp * lf, yMin * lf);
        v += p->amplitude * pv;
    }
    return v;
}
