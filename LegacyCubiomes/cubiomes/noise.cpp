
#include <cmath>
#include <cstdio>
#include <cstring>

#include "LegacyCubiomes/utils/MathHelper.hpp"
#include "LegacyCubiomes/utils/rng.hpp"
#include "generator.hpp"
#include "noise.hpp"


double maintainPrecision(double x) { return x - floor(x / 33554432.0 + 0.5) * 33554432.0; }

ATTR(hot, const, always_inline, artificial)
static inline double indexedLerp(int idx, double a, double b, double c) {
    switch (idx & 0xf) {
        case 0:
            return a + b;
        case 1:
            return -a + b;
        case 2:
            return a - b;
        case 3:
            return -a - b;
        case 4:
            return a + c;
        case 5:
            return -a + c;
        case 6:
            return a - c;
        case 7:
            return -a - c;
        case 8:
            return b + c;
        case 9:
            return -b + c;
        case 10:
            return b - c;
        case 11:
            return -b - c;
        case 12:
            return a + b;
        case 13:
            return -b + c;
        case 14:
            return -a + b;
        case 15:
            return -b - c;
    }
#if __GNUC__
    __builtin_unreachable();
#endif
    return 0;
}

void perlinInit(PerlinNoise* noise, RNG& rng) {
    int i = 0;
    memset(noise, 0, sizeof(*noise));
    noise->a = rng.nextDouble() * 256.0;
    noise->b = rng.nextDouble() * 256.0;
    noise->c = rng.nextDouble() * 256.0;
    noise->amplitude = 1.0;
    noise->lacunarity = 1.0;

    for (i = 0; i < 256; i++) { noise->d[i] = i; }
    for (i = 0; i < 256; i++) {
        int j = rng.nextInt(256 - i) + i;
        uint8_t n = noise->d[i];
        noise->d[i] = noise->d[j];
        noise->d[j] = n;
        noise->d[i + 256] = noise->d[i];
    }
}


double samplePerlin(const Generator* g, const PerlinNoise* noise, double d1, double d2, double d3, double yAmp,
                    double yMin) {
    d1 += noise->a;
    d2 += noise->b;
    d3 += noise->c;

    int i1 = (int) d1 - (int) (d1 < 0);
    int i2 = (int) d2 - (int) (d2 < 0);
    int i3 = (int) d3 - (int) (d3 < 0);

    if (g->getConsole() != CONSOLE::WIIU) {
        d1 -= i1;
        d2 -= i2;
        d3 -= i3;
    }

    double t1 = d1 * d1 * d1 * (d1 * (d1 * 6.0 - 15.0) + 10.0);
    double t2 = d2 * d2 * d2 * (d2 * (d2 * 6.0 - 15.0) + 10.0);
    double t3 = d3 * d3 * d3 * (d3 * (d3 * 6.0 - 15.0) + 10.0);

    if (yAmp != 0.0) {
        double yClamp = yMin < d2 ? yMin : d2;
        d2 -= floor(yClamp / yAmp) * yAmp;
    }

    i1 &= 0xff;
    i2 &= 0xff;
    i3 &= 0xff;

    int a1 = noise->d[i1] + i2;
    int a2 = noise->d[a1] + i3;
    int a3 = noise->d[a1 + 1] + i3;
    int b1 = noise->d[i1 + 1] + i2;
    int b2 = noise->d[b1] + i3;
    int b3 = noise->d[b1 + 1] + i3;

    double l1 = indexedLerp(noise->d[a2], d1, d2, d3);
    double l2 = indexedLerp(noise->d[b2], d1 - 1, d2, d3);
    double l3 = indexedLerp(noise->d[a3], d1, d2 - 1, d3);
    double l4 = indexedLerp(noise->d[b3], d1 - 1, d2 - 1, d3);
    double l5 = indexedLerp(noise->d[a2 + 1], d1, d2, d3 - 1);
    double l6 = indexedLerp(noise->d[b2 + 1], d1 - 1, d2, d3 - 1);
    double l7 = indexedLerp(noise->d[a3 + 1], d1, d2 - 1, d3 - 1);
    double l8 = indexedLerp(noise->d[b3 + 1], d1 - 1, d2 - 1, d3 - 1);

    l1 = MathHelper::lerp(t1, l1, l2);
    l3 = MathHelper::lerp(t1, l3, l4);
    l5 = MathHelper::lerp(t1, l5, l6);
    l7 = MathHelper::lerp(t1, l7, l8);

    l1 = MathHelper::lerp(t2, l1, l3);
    l5 = MathHelper::lerp(t2, l5, l7);

    return MathHelper::lerp(t3, l1, l5);
}


static double simplexGrad(int idx, double x, double y, double z, double d) {
    double con = d - x * x - y * y - z * z;
    if (con < 0) return 0;
    con *= con;
    return con * con * indexedLerp(idx, x, y, z);
}


static double simplexGrad_HARDCODED(int idx, double x, double y) {
    double con = 0.5 - x * x - y * y;
    if (con < 0) return 0;
    con *= con;
    return con * con * indexedLerp(idx, x, y, 0);
}


double sampleSimplex2D(const PerlinNoise* noise, double x, double y) {
    const double SKEW = 0.5 * (sqrt(3) - 1.0);
    const double UNSKEW = (3.0 - sqrt(3)) / 6.0;

    double hf = (x + y) * SKEW;
    int hx = (int) floor(x + hf);
    int hz = (int) floor(y + hf);
    double mhxz = (hx + hz) * UNSKEW;
    double x0 = x - (hx - mhxz);
    double y0 = y - (hz - mhxz);
    int offx = (x0 > y0);
    int offz = !offx;
    double x1 = x0 - offx + UNSKEW;
    double y1 = y0 - offz + UNSKEW;
    double x2 = x0 - 1.0 + 2.0 * UNSKEW;
    double y2 = y0 - 1.0 + 2.0 * UNSKEW;
    int gi0 = noise->d[0xff & (hz)];
    int gi1 = noise->d[0xff & (hz + offz)];
    int gi2 = noise->d[0xff & (hz + 1)];
    gi0 = noise->d[0xff & (gi0 + hx)];
    gi1 = noise->d[0xff & (gi1 + hx + offx)];
    gi2 = noise->d[0xff & (gi2 + hx + 1)];
    double t = 0;
    t += simplexGrad_HARDCODED(gi0 % 12, x0, y0); // , 0.0, 0.5
    t += simplexGrad_HARDCODED(gi1 % 12, x1, y1); // , 0.0, 0.5
    t += simplexGrad_HARDCODED(gi2 % 12, x2, y2); // , 0.0, 0.5
    return 70.0 * t;
}


void octaveInit(OctaveNoise* noise, RNG& rng, PerlinNoise* octaves, int oMin, int len) {
    int i;
    int end = oMin + len - 1;
    double persist = 1.0 / ((1LL << len) - 1.0);
    double lacuna = pow(2.0, end);

    if (len < 1 || end > 0) {
        printf("octavePerlinInit(): unsupported octave range\n");
        return;
    }

    if (end == 0) {
        perlinInit(&octaves[0], rng);
        octaves[0].amplitude = persist;
        octaves[0].lacunarity = lacuna;
        persist *= 2.0;
        lacuna *= 0.5;
        i = 1;
    } else {
        rng.skipNextN(-end * 262);
        i = 0;
    }

    for (; i < len; i++) {
        perlinInit(&octaves[i], rng);
        octaves[i].amplitude = persist;
        octaves[i].lacunarity = lacuna;
        persist *= 2.0;
        lacuna *= 0.5;
    }

    noise->octaves = octaves;
    noise->octcnt = len;
}


double sampleOctave(const Generator* g, const OctaveNoise* noise, double x, double y, double z) {
    double v = 0;
    int i;
    for (i = 0; i < noise->octcnt; i++) {
        PerlinNoise* p = noise->octaves + i;
        double lf = p->lacunarity;
        double ax = maintainPrecision(x * lf);
        double ay = maintainPrecision(y * lf);
        double az = maintainPrecision(z * lf);
        double pv = samplePerlin(g, p, ax, ay, az, 0, 0);
        v += p->amplitude * pv;
    }
    return v;
}


void initSurfaceNoise(SurfaceNoise* sn, DIMENSION dimension, uint64_t worldSeed) {
    RNG rng;
    rng.setSeed(worldSeed);
    octaveInit(&sn->octaveMin, rng, sn->oct + 0, -15, 16);
    octaveInit(&sn->octaveMax, rng, sn->oct + 16, -15, 16);
    octaveInit(&sn->octaveMain, rng, sn->oct + 32, -7, 8);
    if (dimension == DIMENSION::END) {
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


double sampleOctaveAmp(const Generator* g, const OctaveNoise* noise, double x, double y, double z, double yAmp,
                       double yMin, int yDefault) {
    double v = 0;
    int i;
    for (i = 0; i < noise->octcnt; i++) {
        PerlinNoise* p = noise->octaves + i;
        double lf = p->lacunarity;
        double ax = maintainPrecision(x * lf);
        double ay = yDefault ? -p->b : maintainPrecision(y * lf);
        double az = maintainPrecision(z * lf);
        double pv = samplePerlin(g, p, ax, ay, az, yAmp * lf, yMin * lf);
        v += p->amplitude * pv;
    }
    return v;
}
