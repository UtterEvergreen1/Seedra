#pragma once

#include "LegacyCubiomes/utils/MathHelper.hpp"
#include "MapGenBase.hpp"

int64_t CONCAT44(int32_t high, int32_t low) {
    // Shift the 'high' integer to the upper 32 bits and combine it with 'low'.
    return ((int64_t) high << 32) | (uint32_t) low;
}

char* vectorMultiplyHighAndAddSignedHalfWordSaturate(char* a, char* b, char* c) {
    static char var[16];
    return var;
}


typedef long long undefined8;
typedef int undefined4;
typedef char undefined;
typedef unsigned long long ulonglong;
typedef unsigned int uint;
typedef int code;


class UnderwaterCanyonGenerator : public MapGenBase {
    explicit UnderwaterCanyonGenerator(const Generator& generator) : MapGenBase(generator) {}

    void addFeature__23UnderwaterCanyonFeatureFP5LeveliN32P11ChunkPrimer(int param_1, undefined4 param_2, int param_3,
                                                                         int param_4, undefined4 param_5,
                                                                         undefined4 param_6, undefined4 param_7)

    {
        int iVar1;
        uint uVar2;
        double dVar3;
        double dVar4;
        double dVar5;
        double dVar6;
        double dVar7;
        double dVar8;
        double dVar9;
        double dVar10;
        undefined in_vs50[16];
        undefined in_vs57[16];
        undefined in_vs58[16];
        undefined in_vs59[16];
        undefined in_vs60[16];
        undefined in_vs61[16];
        undefined in_vs62[16];
        undefined in_vs63[16];
        undefined8 uVar11;

        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs57, in_vs57, in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs62, in_vs62, in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs60, in_vs60, in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs61, in_vs61, in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs58, in_vs58, in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs59, in_vs59, in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs63, in_vs63, in_vs50);
        iVar1 = nextInt__6RandomFi(param_1 + 8, 0x14);
        if (iVar1 == 0) {
            iVar1 = nextInt__6RandomFi(param_1 + 8, 0x10);
            dVar9 = (double) CONCAT44(0x43300000, param_3 * 0x10 + iVar1 ^ 0x80000000) - 4503601774854144.0;
            iVar1 = nextInt__6RandomFi(param_1 + 8, 0x28);
            iVar1 = nextInt__6RandomFi(param_1 + 8, iVar1 + 8);
            dVar8 = (double) CONCAT44(0x43300000, iVar1 + 0x14U ^ 0x80000000) - 4503601774854144.0;
            uVar2 = nextInt__6RandomFv(param_1 + 8);
            if ((float) ((double) CONCAT44(0x43300000, uVar2 ^ 0x80000000) - 4503601774854144.0) <= 0.2) {
                dVar8 = dVar8 + 15.0;
            }
            iVar1 = nextInt__6RandomFi(param_1 + 8, 0x10);
            dVar7 = (double) CONCAT44(0x43300000, param_4 * 0x10 + iVar1 ^ 0x80000000) - 4503601774854144.0;
            dVar3 = (double) nextFloat__6RandomFv(param_1 + 8);
            dVar5 = (double) ((float) (dVar3 * 3.141592741012573) + (float) (dVar3 * 3.141592741012573));
            dVar3 = (double) nextFloat__6RandomFv(param_1 + 8);
            dVar6 = (double) (((float) (dVar3 - 0.5) + (float) (dVar3 - 0.5)) * 0.125);
            dVar3 = (double) nextFloat__6RandomFv(param_1 + 8);
            dVar10 = 3.0;
            dVar4 = (double) (float) (dVar3 * 3.0);
            dVar3 = (double) nextFloat__6RandomFv(param_1 + 8);
            dVar4 = (double) (float) (dVar3 * dVar10 + dVar4);
            dVar3 = (double) nextFloat__6RandomFv(param_1 + 8);
            if (dVar3 < 0.05000000074505806) { dVar4 = (double) (float) (dVar4 + dVar4); }
            uVar11 = nextLong__6RandomFv(param_1 + 8);
            addTunnel__23UnderwaterCanyonFeatureFP5LevelLiT3P11ChunkPrimerdN26fN29N23T6(
                    dVar9, dVar8, dVar7, dVar4, dVar5, dVar6, 0x4008000000000000, param_1, param_2,
                    (int) ((ulonglong) uVar11 >> 0x20), (int) uVar11, param_5, param_6, param_7, 0, 0);
        }
        instructionSynchronize();
        return;
    }


    void addTunnel__23UnderwaterCanyonFeatureFP5LevelLiT3P11ChunkPrimerdN26fN29N23T6(
            double param_1, double param_2, double param_3, double param_4, double param_5, double param_6,
            double param_7, int param_8, int* param_9_00, undefined4 param_10, undefined4 param_11, int param_12,
            int param_13, undefined4 param_14, uint param_15, uint param_9)

    {
        bool bVar1;
        bool bVar2;
        float fVar3;
        float fVar4;
        uint uVar5;
        bool bVar6;
        int iVar7;
        int iVar8;
        int iVar9;
        undefined4 uVar10;
        undefined4 uVar11;
        undefined4 uVar12;
        undefined4 uVar13;
        undefined4 extraout_r4;
        uint uVar14;
        uint uVar15;
        int iVar16;
        float* pfVar17;
        int iVar18;
        int iVar19;
        uint uVar20;
        double dVar21;
        double dVar22;
        double dVar23;
        double dVar24;
        double dVar25;
        double dVar26;
        double dVar27;
        double dVar28;
        double dVar29;
        double dVar30;
        double dVar31;
        double dVar32;
        double dVar33;
        double dVar34;
        double dVar35;
        undefined in_vs46[16];
        undefined in_vs47[16];
        undefined in_vs48[16];
        undefined in_vs49[16];
        undefined in_vs50[16];
        undefined auVar36[16];
        undefined in_vs51[16];
        undefined in_vs52[16];
        undefined in_vs53[16];
        undefined in_vs54[16];
        undefined in_vs55[16];
        undefined in_vs56[16];
        undefined in_vs57[16];
        undefined in_vs58[16];
        undefined in_vs59[16];
        undefined in_vs60[16];
        undefined in_vs61[16];
        undefined in_vs62[16];
        undefined in_vs63[16];
        float fVar37;
        uint local_228;
        uint local_224;
        uint local_220;
        int local_204;
        int local_200;
        int local_1f4;
        int local_1ec;
        int local_1dc;
        float local_1a8;
        float local_1a4;
        undefined auStack_190[16];
        undefined auStack_180[24];

        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs46, in_vs46, in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs47, in_vs47, in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs48, in_vs48, in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs49, in_vs49, in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs51, in_vs51, in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs52, in_vs52, in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs54, in_vs54, in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs55, in_vs55, in_vs50);
        auVar36 = vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs50, in_vs50, in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs53, in_vs53, auVar36);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs56, in_vs56, auVar36);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs57, in_vs57, auVar36);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs58, in_vs58, auVar36);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs59, in_vs59, auVar36);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs60, in_vs60, auVar36);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs61, in_vs61, auVar36);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs62, in_vs62, auVar36);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs63, in_vs63, auVar36);
        local_228 = param_9;
        MemSect__Fi(0x31);
        __ct__6RandomFL(auStack_180, extraout_r4, param_10, param_11);
        MemSect__Fi(0);
        fVar3 = (float) ((double) CONCAT44(0x43300000, param_12 * 0x10 + 8U ^ 0x80000000) - 4503601774854144.0);
        fVar4 = (float) ((double) CONCAT44(0x43300000, param_13 * 0x10 + 8U ^ 0x80000000) - 4503601774854144.0);
        local_1a8 = 0.0;
        local_1a4 = 0.0;
        __ct__8BlockPosFdN21(param_1, param_2, param_3, auStack_190);
        if ((int) param_9 < 1) {
            iVar7 = nextInt__6RandomFi(auStack_180, 0x1c);
            local_228 = 0x70 - iVar7;
        }
        bVar1 = param_15 == 0xffffffff;
        local_220 = param_15;
        if (bVar1) { local_220 = ((int) local_228 >> 1) + (uint) ((int) local_228 < 0 && (local_228 & 1) != 0); }
        dVar34 = 1.0;
        iVar7 = 0;
        dVar35 = dVar34;
        do {
            dVar21 = (double) nextFloat__6RandomFv(auStack_180);
            dVar22 = (double) nextFloat__6RandomFv(auStack_180);
            fVar37 = (float) (dVar21 * dVar22 + dVar34);
            dVar21 = (double) (fVar37 * fVar37);
            do {
                iVar18 = iVar7 * 4;
                iVar7 = iVar7 + 1;
                *(float*) (iVar18 + param_8 + 0x28) = (float) dVar21;
                if (0x7f < iVar7) {
                    local_1f4 = local_228 - local_220;
                    if ((int) local_228 <= (int) local_220) goto LAB_02a5f83c;
                    dVar34 = (double) ((float) param_4 + 18.0);
                    dVar21 = 0.25;
                    dVar22 = 0.5;
                    goto LAB_02a5eeac;
                }
            } while ((iVar7 != 0) && (iVar18 = nextInt__6RandomFi(auStack_180, 3), iVar18 != 0));
        } while (true);
    LAB_02a5eeac:
        do {
            fVar37 = sinf(
                    ((float) ((double) CONCAT44(0x43300000, local_220 ^ 0x80000000) - 4503601774854144.0) * 3.141593) /
                    (float) ((double) CONCAT44(0x43300000, local_228 ^ 0x80000000) - 4503601774854144.0));
            dVar29 = (double) (fVar37 * (float) param_4 + 1.5);
            dVar31 = (double) (float) (dVar29 * (double) (float) param_7);
            dVar23 = (double) nextFloat__6RandomFv(auStack_180);
            dVar30 = (double) (float) (dVar29 * (double) (float) (dVar23 * dVar21 + 0.75));
            dVar23 = (double) nextFloat__6RandomFv(auStack_180);
            dVar31 = (double) (float) (dVar31 * (double) (float) (dVar23 * dVar21 + 0.75));
            fVar37 = cosf((float) param_6);
            dVar23 = (double) fVar37;
            fVar37 = sinf((float) param_6);
            dVar29 = (double) fVar37;
            fVar37 = cosf((float) param_5);
            param_2 = param_2 + dVar29;
            param_1 = param_1 + (double) (float) ((double) fVar37 * dVar23);
            fVar37 = sinf((float) param_5);
            param_5 = (double) (float) ((double) local_1a4 * 0.05000000074505806 + param_5);
            param_3 = param_3 + (double) (float) ((double) fVar37 * dVar23);
            param_6 = (double) (float) (param_6 * 0.699999988079071 + (double) (local_1a8 * 0.05));
            dVar23 = (double) nextGaussianFloat__6RandomFv(auStack_180);
            dVar29 = (double) nextFloat__6RandomFv(auStack_180);
            local_1a8 = local_1a8 * 0.8 + (float) (dVar23 * dVar29) + (float) (dVar23 * dVar29);
            dVar23 = (double) nextGaussianFloat__6RandomFv(auStack_180);
            dVar29 = (double) nextFloat__6RandomFv(auStack_180);
            local_1a4 = (float) ((double) local_1a4 * dVar22 + (double) ((float) (dVar23 * dVar29) * 4.0));
            if ((bVar1) || (iVar7 = nextInt__6RandomFi(auStack_180, 4), iVar7 != 0)) {
                fVar37 = (float) ((double) CONCAT44(0x43300000, local_228 - local_220 ^ 0x80000000) -
                                  4503601774854144.0);
                if ((float) (dVar34 * dVar34) <
                    -(fVar37 * fVar37 - (((float) param_1 - fVar3) * ((float) param_1 - fVar3) +
                                         ((float) param_3 - fVar4) * ((float) param_3 - fVar4))))
                    break;
                fVar37 = (float) (dVar30 + dVar30);
                if (((((double) ((fVar3 - 16.0) - fVar37) <= param_1) &&
                      ((double) ((fVar4 - 16.0) - fVar37) <= param_3)) &&
                     (param_1 <= (double) (fVar3 + 16.0 + fVar37))) &&
                    (param_3 <= (double) (fVar4 + 16.0 + fVar37))) {
                    dVar23 = floor(param_1 - dVar30);
                    iVar7 = (int) dVar23 + param_12 * -0x10 + -1;
                    dVar23 = floor(param_1 + dVar30);
                    iVar18 = (int) dVar23 + param_12 * -0x10 + 1;
                    dVar23 = floor(param_2 - dVar31);
                    dVar29 = floor(param_2 + dVar31);
                    dVar24 = floor(param_3 - dVar30);
                    local_200 = (int) dVar24 + param_13 * -0x10 + -1;
                    dVar24 = floor(param_3 + dVar30);
                    local_204 = (int) dVar24 + param_13 * -0x10 + 1;
                    if (iVar7 < 0) { iVar7 = 0; }
                    if (0x10 < iVar18) { iVar18 = 0x10; }
                    iVar8 = clamp__3MthSFiN21((int) dVar29 + 1, 1, 0x78);
                    iVar9 = clamp__3MthSFiN21((int) dVar23 + -1, 1, iVar8);
                    iVar16 = _DAT_104c1c48;
                    if (local_200 < 0) { local_200 = 0; }
                    if (0x10 < local_204) { local_204 = 0x10; }
                    iVar19 = *(int*) (_DAT_104c1c48 + 0x8c);
                    uVar10 = defaultBlockState__5BlockFv(_DAT_104c1c48);
                    uVar10 = (**(code**) (iVar19 + 0x54))(iVar16, uVar10);
                    iVar16 = _DAT_104c19bc;
                    iVar19 = *(int*) (_DAT_104c19bc + 0x8c);
                    uVar11 = defaultBlockState__5BlockFv(_DAT_104c19bc);
                    uVar11 = (**(code**) (iVar19 + 0x54))(iVar16, uVar11);
                    iVar16 = _DAT_104c1910;
                    iVar19 = *(int*) (_DAT_104c1910 + 0x8c);
                    uVar12 = defaultBlockState__5BlockFv(_DAT_104c1910);
                    uVar12 = (**(code**) (iVar19 + 0x54))(iVar16, uVar12);
                    iVar16 = _DAT_104c1914;
                    iVar19 = *(int*) (_DAT_104c1914 + 0x8c);
                    uVar13 = defaultBlockState__5BlockFv(_DAT_104c1914);
                    uVar13 = (**(code**) (iVar19 + 0x54))(iVar16, uVar13);
                    local_1ec = iVar18 - iVar7;
                    bVar6 = false;
                    if (iVar7 < iVar18) {
                        local_224 = iVar7 << 0xb;
                        dVar29 = dVar21;
                        dVar23 = (double) (float) dVar35;
                        do {
                            fVar37 = (float) ((double) ((float) ((double) (float) ((double) CONCAT44(
                                                                                           0x43300000,
                                                                                           iVar7 + param_12 * 0x10 ^
                                                                                                   0x80000000) -
                                                                                   4503601774854144.0) +
                                                                 dVar22) -
                                                        (float) param_1) /
                                              dVar30);
                            dVar27 = (double) (fVar37 * fVar37);
                            iVar18 = local_200;
                            dVar24 = dVar29;
                            dVar33 = dVar23;
                            local_1dc = local_204 - local_200;
                            if (local_200 < local_204) {
                                do {
                                    dVar26 =
                                            (double) (float) ((double) ((float) ((double) (float) ((double) CONCAT44(
                                                                                                           0x43300000,
                                                                                                           iVar18 + param_13 * 0x10 ^
                                                                                                                   0x80000000) -
                                                                                                   4503601774854144.0) +
                                                                                 dVar22) -
                                                                        (float) param_3) /
                                                              dVar30);
                                    dVar28 = (double) (float) (dVar26 * dVar26 + dVar27);
                                    dVar26 = dVar23;
                                    dVar32 = dVar29;
                                    if (dVar28 < dVar23) {
                                        uVar20 = iVar8 - 1;
                                        iVar16 = (uVar20 - iVar9) + 1;
                                        if (iVar9 <= (int) uVar20) {
                                            uVar5 = iVar18 * 0x80;
                                            pfVar17 = (float*) (param_8 + 0x28 + uVar20 * 4);
                                            do {
                                                fVar37 =
                                                        (float) ((double) ((float) ((double) (float) ((double) CONCAT44(
                                                                                                              0x43300000,
                                                                                                              uVar20 ^
                                                                                                                      0x80000000) -
                                                                                                      4503601774854144.0) +
                                                                                    dVar22) -
                                                                           (float) param_2) /
                                                                 dVar31);
                                                pfVar17 = pfVar17 + -1;
                                                uVar14 = local_224 | uVar5 | uVar20;
                                                if (((double) (float) (dVar28 * (double) *pfVar17 +
                                                                       (double) ((fVar37 * fVar37) / 6.0)) < dVar23) &&
                                                    ((int) uVar20 < 0x3e)) {
                                                    getBlockId__11ChunkPrimerFi(param_14, uVar14);
                                                    iVar19 = isDiggable__26UnderwaterLargeCaveFeatureSFi();
                                                    if (iVar19 != 0) {
                                                        if ((!bVar6) && (param_9_00 != (int*) 0x0)) {
                                                            (**(code**) (*param_9_00 + 0xbc))(param_9_00, auStack_190);
                                                            iVar19 = getBiomeType__5BiomeCFv();
                                                            if (iVar19 != 10) goto LAB_02a5f83c;
                                                        }
                                                        bVar6 = true;
                                                        if ((int) uVar20 < 10) {
                                                            dVar25 = (double) nextFloat__6RandomFv(auStack_180);
                                                            if (dVar29 <= dVar25) {
                                                                setBlockAndData__11ChunkPrimerFiN21(param_14, uVar14,
                                                                                                    0x31, uVar11);
                                                            } else {
                                                                setBlockAndData__11ChunkPrimerFiN21(param_14, uVar14,
                                                                                                    0xd5, uVar10);
                                                            }
                                                        } else {
                                                            bVar2 = false;
                                                            if (iVar7 < 0xf) {
                                                                uVar15 = local_224 + 0x800 | uVar5 | uVar20;
                                                                iVar19 = getBlockId__11ChunkPrimerFi(param_14, uVar15);
                                                                bVar2 = iVar19 == 0;
                                                                if (bVar2) {
                                                                    setBlockAndData__11ChunkPrimerFiN21(
                                                                            param_14, uVar15, 8, uVar12);
                                                                }
                                                                if (0 < iVar7) goto LAB_02a5f6c4;
                                                            } else {
                                                            LAB_02a5f6c4:
                                                                uVar15 = local_224 - 0x800 | uVar5 | uVar20;
                                                                iVar19 = getBlockId__11ChunkPrimerFi(param_14, uVar15);
                                                                if (iVar19 == 0) {
                                                                    bVar2 = true;
                                                                    setBlockAndData__11ChunkPrimerFiN21(
                                                                            param_14, uVar15, 8, uVar12);
                                                                }
                                                            }
                                                            if (iVar18 < 0xf) {
                                                                uVar15 = local_224 | uVar5 + 0x80 | uVar20;
                                                                iVar19 = getBlockId__11ChunkPrimerFi(param_14, uVar15);
                                                                if (iVar19 == 0) {
                                                                    bVar2 = true;
                                                                    setBlockAndData__11ChunkPrimerFiN21(
                                                                            param_14, uVar15, 8, uVar12);
                                                                }
                                                                if (0 < iVar18) goto LAB_02a5f764;
                                                            } else {
                                                            LAB_02a5f764:
                                                                uVar15 = local_224 | uVar5 - 0x80 | uVar20;
                                                                iVar19 = getBlockId__11ChunkPrimerFi(param_14, uVar15);
                                                                if (iVar19 == 0) {
                                                                    setBlockAndData__11ChunkPrimerFiN21(
                                                                            param_14, uVar15, 8, uVar12);
                                                                    goto LAB_02a5f7d4;
                                                                }
                                                            }
                                                            if (!bVar2) {
                                                                setBlockAndData__11ChunkPrimerFiN21(param_14, uVar14, 9,
                                                                                                    uVar13);
                                                            }
                                                        }
                                                    }
                                                }
                                            LAB_02a5f7d4:
                                                iVar16 = iVar16 + -1;
                                                uVar20 = uVar20 - 1;
                                            } while (iVar16 != 0);
                                        }
                                    }
                                    local_1dc = local_1dc + -1;
                                    iVar18 = iVar18 + 1;
                                    dVar23 = dVar26;
                                    dVar29 = dVar32;
                                } while (local_1dc != 0);
                            }
                            local_1ec = local_1ec + -1;
                            local_224 = local_224 + 0x800;
                            iVar7 = iVar7 + 1;
                            dVar29 = dVar24;
                            dVar23 = dVar33;
                        } while (local_1ec != 0);
                    }
                    if (bVar1) break;
                }
            }
            local_1f4 = local_1f4 + -1;
            local_220 = local_220 + 1;
        } while (local_1f4 != 0);
    LAB_02a5f83c:
        instructionSynchronize();
        return;
    }


    void apply__23UnderwaterCanyonFeatureFP5LeveliT2P11ChunkPrimer(int param_1, undefined4 param_2, int param_3,
                                                                   int param_4, undefined4 param_5)

    {
        int iVar1;
        uint uVar2;
        int iVar3;
        int iVar4;
        int iVar5;
        uint uVar6;
        int iVar7;
        int iVar8;
        uint uVar9;
        uint uVar10;
        uint uVar11;
        uint uVar12;
        uint uVar13;
        uint uVar14;
        uint uVar15;
        int iVar16;
        undefined8 uVar17;
        int local_50;

        uVar17 = getSeed__5LevelFv(param_2);
        setSeed__6RandomFL(param_1 + 8, (int) uVar17, (int) ((ulonglong) uVar17 >> 0x20), (int) uVar17);
        uVar2 = nextInt__6RandomFv(param_1 + 8);
        iVar16 = (((int) uVar2 >> 1) + (uint) ((int) uVar2 < 0 && (uVar2 & 1) != 0)) * 2 + 1;
        uVar2 = nextInt__6RandomFv(param_1 + 8);
        iVar7 = param_3 + -8;
        iVar5 = (((int) uVar2 >> 1) + (uint) ((int) uVar2 < 0 && (uVar2 & 1) != 0)) * 2 + 1;
        if (iVar7 <= param_3 + 8) {
            local_50 = iVar7 * iVar16;
            iVar4 = param_4 + 8;
            iVar3 = param_4 + -8;
            do {
                if (iVar3 <= iVar4) {
                    uVar2 = (iVar4 - iVar3) + 1U >> 3;
                    iVar8 = iVar3;
                    if (uVar2 != 0) {
                        uVar13 = local_50 + (param_4 + -6) * iVar5;
                        uVar6 = local_50 + (param_4 + -1) * iVar5;
                        uVar11 = local_50 + (param_4 + -4) * iVar5;
                        iVar1 = iVar5 * 8;
                        uVar14 = local_50 + (param_4 + -7) * iVar5;
                        uVar12 = local_50 + (param_4 + -5) * iVar5;
                        uVar10 = local_50 + (param_4 + -3) * iVar5;
                        uVar9 = local_50 + (param_4 + -2) * iVar5;
                        uVar15 = local_50 + iVar3 * iVar5;
                        do {
                            uVar17 = getSeed__5LevelFv(param_2);
                            setSeed__6RandomFL(param_1 + 8, (uint) uVar17, (int) uVar15 >> 0x1f ^ (uint) ((ulonglong) uVar17 >> 0x20), uVar15 ^ (uint) uVar17);
                            (**(code**) (*(int*) (param_1 + 0x24) + 0x1c))(param_1, param_2, iVar7, iVar8, param_3, param_4, param_5);
                            uVar17 = getSeed__5LevelFv(param_2);
                            setSeed__6RandomFL(param_1 + 8, (uint) uVar17, (int) uVar14 >> 0x1f ^ (uint) ((ulonglong) uVar17 >> 0x20), uVar14 ^ (uint) uVar17);
                            (**(code**) (*(int*) (param_1 + 0x24) + 0x1c))(param_1, param_2, iVar7, iVar8 + 1, param_3, param_4, param_5);
                            uVar17 = getSeed__5LevelFv(param_2);
                            setSeed__6RandomFL(param_1 + 8, (uint) uVar17, (int) uVar13 >> 0x1f ^ (uint) ((ulonglong) uVar17 >> 0x20), uVar13 ^ (uint) uVar17);
                            (**(code**) (*(int*) (param_1 + 0x24) + 0x1c))(param_1, param_2, iVar7, iVar8 + 2, param_3, param_4, param_5);
                            uVar17 = getSeed__5LevelFv(param_2);
                            setSeed__6RandomFL(param_1 + 8, (uint) uVar17, (int) uVar12 >> 0x1f ^ (uint) ((ulonglong) uVar17 >> 0x20), uVar12 ^ (uint) uVar17);
                            (**(code**) (*(int*) (param_1 + 0x24) + 0x1c))(param_1, param_2, iVar7, iVar8 + 3, param_3, param_4, param_5);
                            uVar17 = getSeed__5LevelFv(param_2);
                            setSeed__6RandomFL(param_1 + 8, (uint) uVar17, (int) uVar11 >> 0x1f ^ (uint) ((ulonglong) uVar17 >> 0x20), uVar11 ^ (uint) uVar17);
                            (**(code**) (*(int*) (param_1 + 0x24) + 0x1c))(param_1, param_2, iVar7, iVar8 + 4, param_3, param_4, param_5);
                            uVar17 = getSeed__5LevelFv(param_2);
                            setSeed__6RandomFL(param_1 + 8, (uint) uVar17, (int) uVar10 >> 0x1f ^ (uint) ((ulonglong) uVar17 >> 0x20), uVar10 ^ (uint) uVar17);
                            (**(code**) (*(int*) (param_1 + 0x24) + 0x1c))(param_1, param_2, iVar7, iVar8 + 5, param_3, param_4, param_5);
                            uVar17 = getSeed__5LevelFv(param_2);
                            setSeed__6RandomFL(param_1 + 8, (uint) uVar17, (int) uVar9 >> 0x1f ^ (uint) ((ulonglong) uVar17 >> 0x20), uVar9 ^ (uint) uVar17);
                            (**(code**) (*(int*) (param_1 + 0x24) + 0x1c))(param_1, param_2, iVar7, iVar8 + 6, param_3, param_4, param_5);
                            uVar17 = getSeed__5LevelFv(param_2);
                            setSeed__6RandomFL(param_1 + 8, (uint) uVar17, (int) uVar6 >> 0x1f ^ (uint) ((ulonglong) uVar17 >> 0x20), uVar6 ^ (uint) uVar17);
                            (**(code**) (*(int*) (param_1 + 0x24) + 0x1c))(param_1, param_2, iVar7, iVar8 + 7, param_3,param_4, param_5);
                            uVar11 = uVar11 + iVar1;
                            uVar12 = uVar12 + iVar1;
                            uVar13 = uVar13 + iVar1;
                            uVar14 = uVar14 + iVar1;
                            uVar15 = uVar15 + iVar1;
                            iVar8 = iVar8 + 8;
                            uVar10 = uVar10 + iVar1;
                            uVar2 = uVar2 - 1;
                            uVar6 = uVar6 + iVar1;
                            uVar9 = uVar9 + iVar1;
                        } while (uVar2 != 0);
                        if (iVar4 < iVar8) goto LAB_02a5ea2c;
                    }
                    uVar2 = local_50 + iVar8 * iVar5;
                    do {
                        uVar17 = getSeed__5LevelFv(param_2);
                        setSeed__6RandomFL(param_1 + 8, (uint) uVar17, (int) uVar2 >> 0x1f ^ (uint) ((ulonglong) uVar17 >> 0x20), uVar2 ^ (uint) uVar17);
                        (**(code**) (*(int*) (param_1 + 0x24) + 0x1c))(param_1, param_2, iVar7, iVar8, param_3, param_4, param_5);
                        iVar8 = iVar8 + 1;
                        uVar2 = uVar2 + iVar5;
                    } while (iVar8 <= iVar4);
                }
            LAB_02a5ea2c:
                iVar7 = iVar7 + 1;
                local_50 = local_50 + iVar16;
            } while (iVar7 <= param_3 + 8);
        }
        return;
    }
};
