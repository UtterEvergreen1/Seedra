#pragma once

#include <string>
#include <cmath>
#include <utility>
#include <vector>

#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "LegacyCubiomes/utils/rng.hpp"
#include "LegacyCubiomes/utils/MathHelper.hpp"

static int64_t lfloor(double value) {
    auto i = (int64_t) value;
    return value < (double) i ? i - 1L : i;
}

class NoiseGeneratorSimplex {
public:
    int grad3[12][3] = {{1,  1,  0},
                        {-1, 1,  0},
                        {1,  -1, 0},
                        {-1, -1, 0},
                        {1,  0,  1},
                        {-1, 0,  1},
                        {1,  0,  -1},
                        {-1, 0,  -1},
                        {0,  1,  1},
                        {0,  -1, 1},
                        {0,  1,  -1},
                        {0,  -1, -1}};
    double SQRT_3 = sqrt(3.0);
    int p[512]{};
    double xo{};
    double yo{};
    double zo{};
    double F2 = 0.5 * (SQRT_3 - 1.0);
    double G2 = (3.0 - SQRT_3) / 6.0;

    void setNoiseGeneratorSimplex(RNG& rng) {
        xo = rng.nextDouble() * 256.0;
        yo = rng.nextDouble() * 256.0;
        zo = rng.nextDouble() * 256.0;

        for (int i = 0; i < 256; i++) {
            p[i] = i;
        }

        for (int l = 0; l < 256; ++l) {
            int j = rng.nextInt(256 - l) + l;
            int k = p[l];
            p[l] = p[j];
            p[j] = k;
            p[l + 256] = p[l];
        }
    }

    static int fastFloor(double value) {
        return value > 0.0 ? (int) value : (int) value - 1;
    }

    static double dot(const int p_151604_0_[], double p_151604_1_, double p_151604_3_) {
        return (double) p_151604_0_[0] * p_151604_1_ + (double) p_151604_0_[1] * p_151604_3_;
    }

    double getValue(double p_151605_1_, double p_151605_3_) {
        double d3 = 0.5 * (SQRT_3 - 1.0);
        double d4 = (p_151605_1_ + p_151605_3_) * d3;
        int i = fastFloor(p_151605_1_ + d4);
        int j = fastFloor(p_151605_3_ + d4);
        double d5 = (3.0 - SQRT_3) / 6.0;
        double d6 = (double) (i + j) * d5;
        double d7 = (double) i - d6;
        double d8 = (double) j - d6;
        double d9 = p_151605_1_ - d7;
        double d10 = p_151605_3_ - d8;
        int k;
        int l;

        if (d9 > d10) {
            k = 1;
            l = 0;
        } else {
            k = 0;
            l = 1;
        }

        double d11 = d9 - (double) k + d5;
        double d12 = d10 - (double) l + d5;
        double d13 = d9 - 1.0 + 2.0 * d5;
        double d14 = d10 - 1.0 + 2.0 * d5;
        int i1 = i & 255;
        int j1 = j & 255;
        int k1 = p[i1 + p[j1]] % 12;
        int l1 = p[i1 + k + p[j1 + l]] % 12;
        int i2 = p[i1 + 1 + p[j1 + 1]] % 12;
        double d15 = 0.5 - d9 * d9 - d10 * d10;
        double d0;

        if (d15 < 0.0) {
            d0 = 0.0;
        } else {
            d15 = d15 * d15;
            d0 = d15 * d15 * dot(grad3[k1], d9, d10);
        }

        double d16 = 0.5 - d11 * d11 - d12 * d12;
        double d1;

        if (d16 < 0.0) {
            d1 = 0.0;
        } else {
            d16 = d16 * d16;
            d1 = d16 * d16 * dot(grad3[l1], d11, d12);
        }

        double d17 = 0.5 - d13 * d13 - d14 * d14;
        double d2;

        if (d17 < 0.0) {
            d2 = 0.0;
        } else {
            d17 = d17 * d17;
            d2 = d17 * d17 * dot(grad3[i2], d13, d14);
        }

        return 70.0 * (d0 + d1 + d2);
    }

    std::vector<double> add(std::vector<double> p_151606_1_, double p_151606_2_,
                            double p_151606_4_, int p_151606_6_, int p_151606_7_,
                            double p_151606_8_, double p_151606_10_, double p_151606_12_) {
        int i = 0;

        for (int j = 0; j < p_151606_7_; ++j) {
            double d0 = (p_151606_4_ + (double) j) * p_151606_10_ + yo;

            for (int k = 0; k < p_151606_6_; ++k) {
                double d1 = (p_151606_2_ + (double) k) * p_151606_8_ + xo;
                double d5 = (d1 + d0) * F2;
                int l = fastFloor(d1 + d5);
                int i1 = fastFloor(d0 + d5);
                double d6 = (double) (l + i1) * G2;
                double d7 = (double) l - d6;
                double d8 = (double) i1 - d6;
                double d9 = d1 - d7;
                double d10 = d0 - d8;
                int j1;
                int k1;

                if (d9 > d10) {
                    j1 = 1;
                    k1 = 0;
                } else {
                    j1 = 0;
                    k1 = 1;
                }

                double d11 = d9 - (double) j1 + G2;
                double d12 = d10 - (double) k1 + G2;
                double d13 = d9 - 1.0 + 2.0 * G2;
                double d14 = d10 - 1.0 + 2.0 * G2;
                int l1 = l & 255;
                int i2 = i1 & 255;
                int j2 = p[l1 + p[i2]] % 12;
                int k2 = p[l1 + j1 + p[i2 + k1]] % 12;
                int l2 = p[l1 + 1 + p[i2 + 1]] % 12;
                double d15 = 0.5 - d9 * d9 - d10 * d10;
                double d2;

                if (d15 < 0.0) {
                    d2 = 0.0;
                } else {
                    d15 = d15 * d15;
                    d2 = d15 * d15 * dot(grad3[j2], d9, d10);
                }

                double d16 = 0.5 - d11 * d11 - d12 * d12;
                double d3;

                if (d16 < 0.0) {
                    d3 = 0.0;
                } else {
                    d16 = d16 * d16;
                    d3 = d16 * d16 * dot(grad3[k2], d11, d12);
                }

                double d17 = 0.5 - d13 * d13 - d14 * d14;
                double d4;

                if (d17 < 0.0) {
                    d4 = 0.0;
                } else {
                    d17 = d17 * d17;
                    d4 = d17 * d17 * dot(grad3[l2], d13, d14);
                }

                int i3 = i++;
                p_151606_1_[i3] += 70.0 * (d2 + d3 + d4) * p_151606_12_;
            }
        }
        return p_151606_1_;
    }
};


class NoiseGeneratorPerlin {
public:
    std::vector<NoiseGeneratorSimplex> noiseLevels;
    int levels;

    void setNoiseGeneratorPerlin(RNG& rng, int p_i45470_2_) {
        levels = p_i45470_2_;
        noiseLevels = std::vector<NoiseGeneratorSimplex>(p_i45470_2_);

        for (int i = 0; i < p_i45470_2_; ++i) {
            noiseLevels[i].setNoiseGeneratorSimplex(rng);
        }
    }

    double getValue(double p_151601_1_, double p_151601_3_) {
        double d0 = 0.0;
        double d1 = 1.0;

        for (int i = 0; i < levels; ++i) {
            d0 += noiseLevels[i].getValue(p_151601_1_ * d1, p_151601_3_ * d1) / d1;
            d1 /= 2.0;
        }

        return d0;
    }

    std::vector<double>
    getRegion(std::vector<double> p_151600_1_, double p_151600_2_, double p_151600_4_, int p_151600_6_, int p_151600_7_,
              double p_151600_8_, double p_151600_10_, double p_151600_12_, double p_151600_14_) {
        //p_151600_8_ = p_151600_8_ / 1.5; // WiiU?
        //p_151600_10_ = p_151600_10_ / 1.5;
        if (!p_151600_1_.empty() && (int) p_151600_1_.size() >= p_151600_6_ * p_151600_7_) {
            for (int i = 0; i < (int) p_151600_1_.size(); ++i) {
                p_151600_1_[i] = 0.0;
            }
        } else {
            p_151600_1_ = std::vector<double>(p_151600_6_ * p_151600_7_);
        }

        double d1 = 1.0;
        double d0 = 1.0;

        for (int j = 0; j < levels; ++j) {
            p_151600_1_ = noiseLevels[j].add(p_151600_1_, p_151600_2_, p_151600_4_, p_151600_6_, p_151600_7_,
                                                   p_151600_8_ * d0, p_151600_10_ * d0, (double) 0.55 / d1);
            d0 *= p_151600_12_;
            d1 *= p_151600_14_;
        }
        return p_151600_1_;
    }

    std::vector<double>
    getRegion(std::vector<double> p_151599_1_, double p_151599_2_, double p_151599_4_, int p_151599_6_, int p_151599_7_,
              double p_151599_8_, double p_151599_10_, double p_151599_12_) {
        return getRegion(std::move(p_151599_1_), p_151599_2_, p_151599_4_, p_151599_6_, p_151599_7_, p_151599_8_,
                               p_151599_10_, p_151599_12_, 0.5);
    }

};

class NoiseGeneratorImproved {
public:
    /**
     * An int[512], where the first 256 elements are the numbers 0..255, in random shuffled order,
     * and the second half of the array is identical to the first half, apparently for convenience in wrapping lookups.
     *
     * Effectively a shuffled 0..255 that wraps once.
     */
    int permutations[512]{};
    double xCoord{};
    double yCoord{};
    double zCoord{};
    double GRAD_X[16] = {1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};
    double GRAD_Y[16] = {1.0, 1.0, -1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0};
    double GRAD_Z[16] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 0.0, 1.0, 0.0, -1.0};
    double GRAD_2X[16] = {1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -1.0, 0.0};
    double GRAD_2Z[16] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 0.0, 1.0, 0.0, -1.0};

    void setNoiseGeneratorImproved(RNG& rng) {
        xCoord = rng.nextDouble() * 256.0;
        yCoord = rng.nextDouble() * 256.0;
        zCoord = rng.nextDouble() * 256.0;

        // TODO: this could definitely be statically loaded with a memcpy()
        for (int i = 0; i < 256; i++) {
            permutations[i] = i;
        }

        for (int l = 0; l < 256; ++l) {
            int j = rng.nextInt(256 - l) + l;
            int k = permutations[l];
            permutations[l] = permutations[j];
            permutations[j] = k;
            permutations[l + 256] = permutations[l];
        }
    }

    double grad(int p_76310_1_, double p_76310_2_, double p_76310_4_, double p_76310_6_) {
        int i = p_76310_1_ & 15;
        return GRAD_X[i] * p_76310_2_ + GRAD_Y[i] * p_76310_4_ + GRAD_Z[i] * p_76310_6_;
    }

    double grad2(int p_76309_1_, double p_76309_2_, double p_76309_4_) {
        int i = p_76309_1_ & 15;
        return GRAD_2X[i] * p_76309_2_ + GRAD_2Z[i] * p_76309_4_;
    }

    /**
     * noiseArray should be xSize*ySize*zSize in size
     */
    std::vector<double>
    populateNoiseArray(std::vector<double> noiseArray, double xOffset, double yOffset, double zOffset, int xSize,
                       int ySize, int zSize, double xScale, double yScale, double zScale, double noiseScale) {
        if (ySize == 1) {
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
                // double d17 = xOffset + (double)j2 * xScale + xCoord; // 8942.070107
                int i6 = (int) d17;

                if (d17 < (double) i6) {
                    --i6;
                }

                int k2 = i6 & 255;
                //d17 = d17 - (double)i6; // only on xbox (and java)
                double d18 = d17 * d17 * d17 * (d17 * (d17 * 6.0 - 15.0) + 10.0); // 53637.420644

                for (int j6 = 0; j6 < zSize; ++j6) {
                    double d19 = zOffset + (double) j6 * zScale + zCoord; // d17 should equal 12973.405044
                    int k6 = (int) d19;

                    if (d19 < (double) k6) {
                        --k6;
                    }

                    int l6 = k6 & 255;
                    //d19 = d19 - (double)k6; // only on xbox (and java)
                    double d20 = d19 * d19 * d19 * (d19 * (d19 * 6.0 - 15.0) + 10.0);
                    i5 = permutations[k2] + 0;
                    j5 = permutations[i5] + l6;
                    j = permutations[k2 + 1] + 0;
                    k5 = permutations[j] + l6;
                    d14 = MathHelper::lerp(d18,
                               grad2(permutations[j5], d17, d19),
                               grad(permutations[k5], d17 - 1.0, 0.0, d19));
                    d15 = MathHelper::lerp(d18,
                               grad(permutations[j5 + 1], d17, 0.0, d19 - 1.0),
                               grad(permutations[k5 + 1], d17 - 1.0, 0.0, d19 - 1.0));
                    double d21 = MathHelper::lerp(d20, d14, d15);
                    int i7 = l5++;
                    noiseArray[i7] += d21 * d16;
                }
                d17 += i6 + xScale;
            }
        } else {
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

                if (d5 < (double) i3) {
                    --i3;
                }

                int j3 = i3 & 255;
                d5 = d5 - (double) i3;
                double d6 = d5 * d5 * d5 * (d5 * (d5 * 6.0 - 15.0) + 10.0);

                for (int k3 = 0; k3 < zSize; ++k3) {
                    double d7 = zOffset + (double) k3 * zScale + zCoord;
                    int l3 = (int) d7;

                    if (d7 < (double) l3) {
                        --l3;
                    }

                    int i4 = l3 & 255;
                    d7 = d7 - (double) l3;
                    double d8 = d7 * d7 * d7 * (d7 * (d7 * 6.0 - 15.0) + 10.0);

                    for (int j4 = 0; j4 < ySize; j4++) {
                        double d9 = yOffset + (double) j4 * yScale + yCoord;
                        int k4 = (int) d9;

                        if (d9 < (double) k4) {
                            --k4;
                        }

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
                            d1 = MathHelper::lerp(d6,
                                      grad(permutations[i1], d5, d9, d7),
                                      grad(permutations[l1], d5 - 1.0, d9, d7));
                            d2 = MathHelper::lerp(d6,
                                      grad(permutations[j1], d5, d9 - 1.0, d7),
                                      grad(permutations[i2], d5 - 1.0, d9 - 1.0, d7));
                            d3 = MathHelper::lerp(d6,
                                      grad(permutations[i1 + 1], d5, d9, d7 - 1.0),
                                      grad(permutations[l1 + 1], d5 - 1.0, d9, d7 - 1.0));
                            d4 = MathHelper::lerp(d6,
                                      grad(permutations[j1 + 1], d5, d9 - 1.0, d7 - 1.0),
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

class NoiseGeneratorOctaves {
public:
    /**
     * Collection of noise generation functions.  Output is combined to produce different octaves of noise.
     */
    std::vector<NoiseGeneratorImproved> generatorCollection;
    int octaves;

    void setNoiseGeneratorOctaves(RNG& rng, int octavesIn) {
        octaves = octavesIn;
        generatorCollection = std::vector<NoiseGeneratorImproved>(octavesIn);

        for (int i = 0; i < octavesIn; ++i) {
            generatorCollection[i].setNoiseGeneratorImproved(rng);
        }
    }

    /**
     * pars:(par2,3,4=noiseOffset ; so that adjacent noise segments connect) (pars5,6,7=x,y,zArraySize),(pars8,10,12 =
     * x,y,z noiseScale)
     */
    std::vector<double>
    genNoiseOctaves(std::vector<double> noiseArray,
                    int xOffset, int yOffset, int zOffset,
                    int xSize, int ySize, int zSize,
                    double xScale, double yScale, double zScale) {
        if (noiseArray.empty()) {
            noiseArray.resize(xSize * ySize * zSize);
        } else {
            for (int i = 0; i < sizeof(noiseArray); ++i) {
                noiseArray[i] = 0.0;
            }
        }

        double d3 = 1.0;

        for (int j = 0; j < octaves; ++j) {
            double d0 = (double) xOffset * d3 * xScale;
            double d1 = (double) yOffset * d3 * yScale;
            double d2 = (double) zOffset * d3 * zScale;
            int64_t k = lfloor(d0);
            int64_t l = lfloor(d2);
            d0 = d0 - (double) k;
            d2 = d2 - (double) l;
            k = k % 16777216LL;
            l = l % 16777216LL;
            d0 = d0 + (double) k;
            d2 = d2 + (double) l;
            noiseArray = generatorCollection[j].populateNoiseArray(noiseArray,
                                                                   d0, d1, d2,
                                                                   xSize, ySize, zSize,
                                                                   xScale * d3, yScale * d3, zScale * d3,
                                                                   d3);
            d3 /= 2.0;
        }
        return noiseArray;
    }

    /**
     * Bouncer function to the main one with some default arguments.
     */
    std::vector<double>
    genNoiseOctaves(std::vector<double> noiseArray, int xOffset, int zOffset, int xSize, int zSize, double xScale,
                    double zScale, double p_76305_10_) {
        return genNoiseOctaves(std::move(noiseArray), xOffset, 10, zOffset, xSize, 1, zSize, xScale, 1.0, zScale);
    }
};
