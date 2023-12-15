#pragma once

#include "LegacyCubiomes/utils/MathHelper.hpp"
#include "MapGenBase.hpp"
#include <cmath>

// UnderwaterLargeCaveFeature


class UnderwaterLargeCaveGenerator : public MapGenBase {
public:
    explicit UnderwaterLargeCaveGenerator(const Generator& generator) : MapGenBase(generator) {}

    UnderwaterLargeCaveGenerator(CONSOLE console, LCEVERSION version,
                                 int64_t seed, WORLDSIZE size, BIOMESCALE scale)
        : MapGenBase(console, version, seed, size, scale) {}


    void addFeature() {}
    /*
    void addFeature__26UnderwaterLargeCaveFeatureFP5LeveliN32P11ChunkPrimer
          (int param_1,undefined4 param_2,int param_3,int param_4,undefined4 param_5,
          undefined4 param_6,undefined4 param_7) {
       int iVar1;
       int iVar2;
       uint uVar3;
       uint uVar4;
       double dVar5;
       double dVar6;
       double dVar7;
       double dVar8;
       double dVar9;
       double dVar10;
       double dVar11;
       double dVar12;
       double dVar13;
       double dVar14;
       undefined8 uVar15;
       double dVar16;
       double dVar17;
       double dVar18;
       undefined in_vs50 [16];
       undefined auVar19 [16];
       undefined in_vs51 [16];
       undefined in_vs52 [16];
       undefined in_vs53 [16];
       undefined in_vs54 [16];
       undefined in_vs55 [16];
       undefined in_vs56 [16];
       undefined in_vs57 [16];
       undefined in_vs58 [16];
       undefined in_vs59 [16];
       undefined in_vs60 [16];
       undefined in_vs61 [16];
       undefined in_vs62 [16];
       undefined in_vs63 [16];
       undefined8 uVar20;
       undefined8 uVar21;

       auVar19 = vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs50,in_vs50,in_vs50);
       vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs51,in_vs51,auVar19);
       vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs52,in_vs52,auVar19);
       vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs53,in_vs53,auVar19);
       vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs55,in_vs55,auVar19);
       vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs56,in_vs56,auVar19);
       vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs58,in_vs58,auVar19);
       vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs59,in_vs59,auVar19);
       vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs54,in_vs54,auVar19);
       vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs57,in_vs57,auVar19);
       vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs60,in_vs60,auVar19);
       vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs61,in_vs61,auVar19);
       vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs62,in_vs62,auVar19);
       vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs63,in_vs63,auVar19);
       iVar1 = nextInt__6RandomFi(param_1 + 8,0x28);
       iVar1 = nextInt__6RandomFi(param_1 + 8,iVar1 + 1);
       iVar1 = nextInt__6RandomFi(param_1 + 8,iVar1 + 1);
       iVar2 = nextInt__6RandomFi(param_1 + 8,0xf);
       if (iVar2 != 0) {
           iVar1 = 0;
       }
       if (0 < iVar1) {
           dVar17 = 3.141592741012573;
           uVar15 = 0x3ff0000000000000;
           dVar18 = 0.5;
           dVar16 = 0.125;
           do {
               iVar2 = nextInt__6RandomFi(param_1 + 8,0x10);
               dVar11 = (double)CONCAT44(0x43300000,param_4 * 0x10 + iVar2 ^ 0x80000000) - 4503601774854144.0
                       ;
               iVar2 = nextInt__6RandomFi(param_1 + 8,0x78);
               uVar3 = nextInt__6RandomFi(param_1 + 8,iVar2 + 8);
               dVar10 = (double)CONCAT44(0x43300000,uVar3 ^ 0x80000000) - 4503601774854144.0;
               iVar2 = nextInt__6RandomFi(param_1 + 8,0x10);
               uVar3 = 1;
               dVar8 = (double)CONCAT44(0x43300000,param_3 * 0x10 + iVar2 ^ 0x80000000) - 4503601774854144.0;
               iVar2 = nextInt__6RandomFi(param_1 + 8,4);
               if (iVar2 == 0) {
                   uVar20 = nextLong__6RandomFv(param_1 + 8);
                   addRoom__26UnderwaterLargeCaveFeatureFP5LevelLiT3P11ChunkPrimerdN26
                           (dVar8,dVar10,dVar11,param_1,param_2,(int)((ulonglong)uVar20 >> 0x20),(int)uVar20,
                            param_5,param_6,param_7);
                   iVar2 = nextInt__6RandomFi(param_1 + 8,4);
                   uVar3 = iVar2 + 1;
               }
               if (0 < (int)uVar3) {
                   uVar20 = uVar15;
                   dVar12 = dVar16;
                   dVar14 = dVar17;
                   dVar13 = dVar18;
                   for (uVar4 = uVar3 >> 2; uVar4 != 0; uVar4 = uVar4 - 1) {
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar7 = (double)((float)(dVar5 * dVar17) + (float)(dVar5 * dVar17));
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar6 = (double)(float)((double)((float)(dVar5 - dVar18) + (float)(dVar5 - dVar18)) *
                                                 dVar16);
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar9 = (double)(float)(dVar5 + dVar5);
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar5 = (double)(float)(dVar9 + dVar5);
                       uVar21 = nextLong__6RandomFv(param_1 + 8);
                       addTunnel__26UnderwaterLargeCaveFeatureFP5LevelLiT3P11ChunkPrimerdN26fN29N23T6
                               (dVar8,dVar10,dVar11,dVar5,dVar7,dVar6,uVar15,param_1,param_2,
                                (int)((ulonglong)uVar21 >> 0x20),(int)uVar21,param_5,param_6,param_7,0,0);
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar6 = (double)((float)(dVar5 * dVar17) + (float)(dVar5 * dVar17));
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar7 = (double)(float)((double)((float)(dVar5 - dVar18) + (float)(dVar5 - dVar18)) *
                                                 dVar16);
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar9 = (double)(float)(dVar5 + dVar5);
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar5 = (double)(float)(dVar9 + dVar5);
                       uVar21 = nextLong__6RandomFv(param_1 + 8);
                       addTunnel__26UnderwaterLargeCaveFeatureFP5LevelLiT3P11ChunkPrimerdN26fN29N23T6
                               (dVar8,dVar10,dVar11,dVar5,dVar6,dVar7,uVar15,param_1,param_2,
                                (int)((ulonglong)uVar21 >> 0x20),(int)uVar21,param_5,param_6,param_7,0,0);
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar6 = (double)((float)(dVar5 * dVar17) + (float)(dVar5 * dVar17));
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar7 = (double)(float)((double)((float)(dVar5 - dVar18) + (float)(dVar5 - dVar18)) *
                                                 dVar16);
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar9 = (double)(float)(dVar5 + dVar5);
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar5 = (double)(float)(dVar9 + dVar5);
                       uVar21 = nextLong__6RandomFv(param_1 + 8);
                       addTunnel__26UnderwaterLargeCaveFeatureFP5LevelLiT3P11ChunkPrimerdN26fN29N23T6
                               (dVar8,dVar10,dVar11,dVar5,dVar6,dVar7,uVar15,param_1,param_2,
                                (int)((ulonglong)uVar21 >> 0x20),(int)uVar21,param_5,param_6,param_7,0,0);
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar6 = (double)((float)(dVar5 * dVar17) + (float)(dVar5 * dVar17));
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar7 = (double)(float)((double)((float)(dVar5 - dVar18) + (float)(dVar5 - dVar18)) *
                                                 dVar16);
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar9 = (double)(float)(dVar5 + dVar5);
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar5 = (double)(float)(dVar9 + dVar5);
                       uVar21 = nextLong__6RandomFv(param_1 + 8);
                       addTunnel__26UnderwaterLargeCaveFeatureFP5LevelLiT3P11ChunkPrimerdN26fN29N23T6
                               (dVar8,dVar10,dVar11,dVar5,dVar6,dVar7,uVar15,param_1,param_2,
                                (int)((ulonglong)uVar21 >> 0x20),(int)uVar21,param_5,param_6,param_7,0,0);
                   }
                   uVar15 = uVar20;
                   dVar16 = dVar12;
                   dVar17 = dVar14;
                   dVar18 = dVar13;
                   for (uVar3 = uVar3 & 3; uVar3 != 0; uVar3 = uVar3 - 1) {
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar7 = (double)((float)(dVar5 * dVar14) + (float)(dVar5 * dVar14));
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar6 = (double)(float)((double)((float)(dVar5 - dVar13) + (float)(dVar5 - dVar13)) *
                                                 dVar12);
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar9 = (double)(float)(dVar5 + dVar5);
                       dVar5 = (double)nextFloat__6RandomFv(param_1 + 8);
                       dVar5 = (double)(float)(dVar9 + dVar5);
                       uVar21 = nextLong__6RandomFv(param_1 + 8);
                       addTunnel__26UnderwaterLargeCaveFeatureFP5LevelLiT3P11ChunkPrimerdN26fN29N23T6
                               (dVar8,dVar10,dVar11,dVar5,dVar7,dVar6,uVar20,param_1,param_2,
                                (int)((ulonglong)uVar21 >> 0x20),(int)uVar21,param_5,param_6,param_7,0,0);
                   }
               }
               iVar1 = iVar1 + -1;
           } while (iVar1 != 0);
       }
       instructionSynchronize();
       return;
    }
     */

    void addRoom() {}
    /*

    void addRoom__26UnderwaterLargeCaveFeatureFP5LevelLiT3P11ChunkPrimerdN26
              (undefined8 param_1,undefined8 param_2,undefined8 param_3,int param_4,
              undefined4 param_5,undefined4 param_6,undefined4 param_7,undefined4 param_8,
              undefined4 param_9,undefined4 param_10) {
       double dVar1;
       undefined8 uVar2;
       undefined in_vs50 [16];
       undefined in_vs60 [16];
       undefined in_vs61 [16];
       undefined in_vs62 [16];
       undefined in_vs63 [16];

       vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs62,in_vs62,in_vs50);
       vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs60,in_vs60,in_vs50);
       vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs61,in_vs61,in_vs50);
       vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs63,in_vs63,in_vs50);
       uVar2 = 0;
       dVar1 = (double)nextFloat__6RandomFv(param_4 + 8);
       addTunnel__26UnderwaterLargeCaveFeatureFP5LevelLiT3P11ChunkPrimerdN26fN29N23T6
               (param_1,param_2,param_3,(double)(float)(dVar1 * 6.0 + 1.0),uVar2,uVar2,
                0x3fe0000000000000,param_4,param_5,param_6,param_7,param_8,param_9,param_10,0xffffffff,
                0xffffffff);
       instructionSynchronize();
       return;
    }
     */

    void addTunnel() {}
    /*
    void addTunnel__26UnderwaterLargeCaveFeatureFP5LevelLiT3P11ChunkPrimerdN26fN29N23T6
           (double param_1,double param_2,double param_3,double param_4,double param_5,
           double param_6,double param_7,undefined4 param_8,int *param_9_00,undefined4 param_10,
           undefined4 param_11,int param_12,int param_13,undefined4 param_14,uint param_15,
           uint param_9) {
        bool bVar1;
        float fVar2;
        float fVar3;
        uint uVar4;
        uint uVar5;
        float fVar6;
        bool bVar7;
        bool bVar8;
        int iVar9;
        int iVar10;
        int iVar11;
        undefined4 uVar12;
        undefined4 uVar13;
        undefined4 uVar14;
        undefined4 uVar15;
        undefined4 uVar16;
        undefined4 extraout_r4;
        uint uVar17;
        uint uVar18;
        int iVar19;
        int iVar20;
        int iVar21;
        int iVar22;
        uint uVar23;
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
        double dVar36;
        double dVar37;
        double dVar38;
        undefined in_vs46 [16];
        undefined in_vs47 [16];
        undefined in_vs48 [16];
        undefined in_vs49 [16];
        undefined in_vs50 [16];
        undefined auVar39 [16];
        undefined in_vs51 [16];
        undefined in_vs52 [16];
        undefined in_vs53 [16];
        undefined in_vs54 [16];
        undefined in_vs55 [16];
        undefined in_vs56 [16];
        undefined in_vs57 [16];
        undefined in_vs58 [16];
        undefined in_vs59 [16];
        undefined in_vs60 [16];
        undefined in_vs61 [16];
        undefined in_vs62 [16];
        undefined in_vs63 [16];
        undefined8 uVar40;
        float fVar41;
        int local_20c;
        int local_208;
        uint local_1f4;
        uint local_1f0;
        int local_1ec;
        float local_1e8;
        int local_1e0;
        int local_1dc;
        uint local_1bc;
        float local_1ac;
        float local_1a8;
        undefined auStack_190 [16];
        undefined auStack_180 [24];

        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs46,in_vs46,in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs47,in_vs47,in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs48,in_vs48,in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs49,in_vs49,in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs51,in_vs51,in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs52,in_vs52,in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs54,in_vs54,in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs55,in_vs55,in_vs50);
        auVar39 = vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs50,in_vs50,in_vs50);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs53,in_vs53,auVar39);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs56,in_vs56,auVar39);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs57,in_vs57,auVar39);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs58,in_vs58,auVar39);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs59,in_vs59,auVar39);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs60,in_vs60,auVar39);
        fVar6 = (float)param_4;
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs61,in_vs61,auVar39);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs62,in_vs62,auVar39);
        vectorMultiplyHighAndAddSignedHalfWordSaturate(in_vs63,in_vs63,auVar39);
        local_1e8 = (float)param_5;
        local_1f4 = param_9;
        local_1a8 = 0.0;
        fVar2 = (float)((double)CONCAT44(0x43300000,param_12 * 0x10 + 8U ^ 0x80000000) -
                         4503601774854144.0);
        local_1ac = 0.0;
        fVar3 = (float)((double)CONCAT44(0x43300000,param_13 * 0x10 + 8U ^ 0x80000000) -
                         4503601774854144.0);
        __ct__8BlockPosFdN21(auStack_190);
        MemSect__Fi(0x31);
        __ct__6RandomFL(auStack_180,extraout_r4,param_10,param_11);
        MemSect__Fi(0);
        if ((int)param_9 < 1) {
            iVar9 = nextInt__6RandomFi(auStack_180,0x1c);
            local_1f4 = 0x70 - iVar9;
        }
        bVar8 = false;
        if (param_15 == 0xffffffff) {
            local_1f0 = ((int)local_1f4 >> 1) + (uint)((int)local_1f4 < 0 && (local_1f4 & 1) != 0);
            bVar8 = true;
            iVar9 = nextInt__6RandomFi(auStack_180);
            local_1bc = iVar9 + ((int)local_1f4 >> 2) + (uint)((int)local_1f4 < 0 && (local_1f4 & 3) != 0);
            iVar9 = nextInt__6RandomFi(auStack_180,6);
            local_1ec = -local_1f0;
            if ((int)local_1f4 <= (int)local_1f0) goto LAB_02a61184;
        }
        else {
            iVar9 = nextInt__6RandomFi(auStack_180,
                                       ((int)local_1f4 >> 1) +
                                               (uint)((int)local_1f4 < 0 && (local_1f4 & 1) != 0));
            local_1bc = iVar9 + ((int)local_1f4 >> 2) + (uint)((int)local_1f4 < 0 && (local_1f4 & 3) != 0);
            iVar9 = nextInt__6RandomFi(auStack_180,6);
            local_1ec = -param_15;
            local_1f0 = param_15;
            if ((int)local_1f4 <= (int)param_15) goto LAB_02a61184;
        }
        local_1ec = local_1f4 + local_1ec;
        dVar38 = 0.5;
        do {
            fVar41 = sinf(((float)((double)CONCAT44(0x43300000,local_1f0 ^ 0x80000000) - 4503601774854144.0)
                           * 3.141593) /
                          (float)((double)CONCAT44(0x43300000,local_1f4 ^ 0x80000000) - 4503601774854144.0))
                    ;
            dVar32 = (double)(fVar41 * fVar6 + 1.5);
            dVar33 = (double)(float)(dVar32 * (double)(float)param_7);
            fVar41 = cosf((float)param_6);
            dVar24 = (double)fVar41;
            fVar41 = sinf((float)param_6);
            dVar25 = (double)fVar41;
            fVar41 = cosf(local_1e8);
            param_2 = param_2 + dVar25;
            param_1 = param_1 + (double)(float)((double)fVar41 * dVar24);
            fVar41 = sinf(local_1e8);
            param_3 = param_3 + (double)(float)((double)fVar41 * dVar24);
            if (iVar9 == 0) {
                dVar36 = 0.8999999761581421;
                local_1e8 = local_1a8 * 0.1 + local_1e8;
                dVar34 = 0.75;
                param_6 = (double)(local_1ac * 0.1 + (float)(param_6 * 0.9200000166893005));
                dVar24 = (double)nextGaussianFloat__6RandomFv(auStack_180);
                dVar25 = (double)nextFloat__6RandomFv(auStack_180);
                local_1ac = (float)((double)local_1ac * dVar36 +
                                     (double)((float)(dVar24 * dVar25) + (float)(dVar24 * dVar25)));
                dVar24 = (double)nextGaussianFloat__6RandomFv(auStack_180);
                dVar25 = (double)nextFloat__6RandomFv(auStack_180);
                local_1a8 = (float)((double)local_1a8 * dVar34 + (double)((float)(dVar24 * dVar25) * 4.0));
                if (bVar8) goto LAB_02a60984;
            LAB_02a60774:
                if ((local_1f0 == local_1bc) && (1.0 < fVar6)) {
                    uVar40 = nextLong__6RandomFv(auStack_180);
                    dVar24 = (double)nextFloat__6RandomFv(auStack_180);
                    dVar25 = (double)(float)(param_6 / 3.0);
                    addTunnel__26UnderwaterLargeCaveFeatureFP5LevelLiT3P11ChunkPrimerdN26fN29N23T6
                            (param_1,param_2,param_3,(double)(float)(dVar24 * dVar38 + dVar38),
                             (double)(local_1e8 - 1.570796),dVar25,0x3ff0000000000000,param_8,param_9_00,
                             (int)((ulonglong)uVar40 >> 0x20),(int)uVar40,param_12,param_13,param_14,local_1f0
                             ,local_1f4);
                    uVar40 = nextLong__6RandomFv(auStack_180);
                    dVar24 = (double)nextFloat__6RandomFv(auStack_180);
                    addTunnel__26UnderwaterLargeCaveFeatureFP5LevelLiT3P11ChunkPrimerdN26fN29N23T6
                            (param_1,param_2,param_3,(double)(float)(dVar24 * dVar38 + dVar38),
                             (double)(local_1e8 + 1.570796),dVar25,0x3ff0000000000000,param_8,param_9_00,
                             (int)((ulonglong)uVar40 >> 0x20),(int)uVar40,param_12,param_13,param_14,local_1f0
                             ,local_1f4);
                    instructionSynchronize();
                    return;
                }
                iVar22 = nextInt__6RandomFi(auStack_180,4);
                if (iVar22 != 0) goto LAB_02a60984;
            }
            else {
                dVar36 = 0.8999999761581421;
                local_1e8 = local_1a8 * 0.1 + local_1e8;
                dVar34 = 0.75;
                param_6 = (double)(local_1ac * 0.1 + (float)(param_6 * 0.699999988079071));
                dVar24 = (double)nextGaussianFloat__6RandomFv(auStack_180);
                dVar25 = (double)nextFloat__6RandomFv(auStack_180);
                local_1ac = (float)((double)local_1ac * dVar36 +
                                     (double)((float)(dVar24 * dVar25) + (float)(dVar24 * dVar25)));
                dVar24 = (double)nextGaussianFloat__6RandomFv(auStack_180);
                dVar25 = (double)nextFloat__6RandomFv(auStack_180);
                local_1a8 = (float)((double)local_1a8 * dVar34 + (double)((float)(dVar24 * dVar25) * 4.0));
                if (!bVar8) goto LAB_02a60774;
            LAB_02a60984:
                fVar41 = (float)((double)CONCAT44(0x43300000,local_1f4 - local_1f0 ^ 0x80000000) -
                                  4503601774854144.0);
                if ((fVar6 + 18.0) * (fVar6 + 18.0) <
                    -(fVar41 * fVar41 -
                      (((float)param_1 - fVar2) * ((float)param_1 - fVar2) +
                       ((float)param_3 - fVar3) * ((float)param_3 - fVar3)))) break;
                fVar41 = (float)(dVar32 + dVar32);
                if (((((double)((fVar2 - 16.0) - fVar41) <= param_1) &&
                      ((double)((fVar3 - 16.0) - fVar41) <= param_3)) &&
                     (param_1 <= (double)(fVar2 + 16.0 + fVar41))) &&
                    (param_3 <= (double)(fVar3 + 16.0 + fVar41))) {
                    dVar24 = floor(param_1 - dVar32);
                    iVar22 = (int)dVar24 + param_12 * -0x10 + -1;
                    dVar24 = floor(param_1 + dVar32);
                    iVar20 = (int)dVar24 + param_12 * -0x10 + 1;
                    dVar24 = floor(param_2 - dVar33);
                    dVar25 = floor(param_2 + dVar33);
                    dVar34 = floor(param_3 - dVar32);
                    local_208 = (int)dVar34 + param_13 * -0x10 + -1;
                    dVar34 = floor(param_3 + dVar32);
                    local_20c = (int)dVar34 + param_13 * -0x10 + 1;
                    if (iVar22 < 0) {
                        iVar22 = 0;
                    }
                    if (0x10 < iVar20) {
                        iVar20 = 0x10;
                    }
                    iVar10 = clamp__3MthSFiN21((int)dVar25 + 1,1,0x78);
                    iVar11 = clamp__3MthSFiN21((int)dVar24 + -1,1,iVar10);
                    iVar19 = _DAT_104c1c48;
                    if (local_208 < 0) {
                        local_208 = 0;
                    }
                    if (0x10 < local_20c) {
                        local_20c = 0x10;
                    }
                    iVar21 = *(int *)(_DAT_104c1c48 + 0x8c);
                    uVar12 = defaultBlockState__5BlockFv(_DAT_104c1c48);
                    uVar12 = (**(code **)(iVar21 + 0x54))(iVar19,uVar12);
                    iVar19 = _DAT_104c19bc;
                    iVar21 = *(int *)(_DAT_104c19bc + 0x8c);
                    uVar13 = defaultBlockState__5BlockFv(_DAT_104c19bc);
                    uVar13 = (**(code **)(iVar21 + 0x54))(iVar19,uVar13);
                    iVar19 = _DAT_104c1910;
                    iVar21 = *(int *)(_DAT_104c1910 + 0x8c);
                    uVar14 = defaultBlockState__5BlockFv(_DAT_104c1910);
                    uVar14 = (**(code **)(iVar21 + 0x54))(iVar19,uVar14);
                    iVar19 = _DAT_104c1914;
                    iVar21 = *(int *)(_DAT_104c1914 + 0x8c);
                    uVar15 = defaultBlockState__5BlockFv(_DAT_104c1914);
                    uVar15 = (**(code **)(iVar21 + 0x54))(iVar19,uVar15);
                    iVar19 = _DAT_104c191c;
                    iVar21 = *(int *)(_DAT_104c191c + 0x8c);
                    uVar16 = defaultBlockState__5BlockFv(_DAT_104c191c);
                    uVar16 = (**(code **)(iVar21 + 0x54))(iVar19,uVar16);
                    bVar7 = false;
                    local_1e0 = iVar20 - iVar22;
                    if (iVar22 < iVar20) {
                        dVar24 = 1.0;
                        dVar25 = 0.25;
                        dVar34 = -0.7;
                        do {
                            fVar41 = (float)((double)((float)((double)(float)((double)CONCAT44(0x43300000,
                                                                                                     iVar22 + param_12 *
                                                                                                                             0x10 ^
                                                                                                             0x80000000) -
                                                                                   4503601774854144.0) + dVar38) -
                                                        (float)param_1) / dVar32);
                            dVar28 = (double)(fVar41 * fVar41);
                            iVar20 = local_208;
                            dVar36 = dVar24;
                            dVar35 = dVar25;
                            dVar37 = dVar34;
                            local_1dc = local_20c - local_208;
                            if (local_208 < local_20c) {
                                do {
                                    fVar41 = (float)((double)((float)((double)(float)((double)CONCAT44(0x43300000,
                                                                                                             iVar20 + param_13
                                                                                                                                     * 0x10 ^
                                                                                                                     0x80000000) -
                                                                                           4503601774854144.0) + dVar38) -
                                                                (float)param_3) / dVar32);
                                    dVar27 = (double)(fVar41 * fVar41);
                                    dVar29 = dVar24;
                                    dVar30 = dVar25;
                                    dVar31 = dVar34;
                                    if ((double)(float)(dVar28 + dVar27) < dVar24) {
                                        uVar23 = iVar10 - 1;
                                        iVar19 = (uVar23 - iVar11) + 1;
                                        if (iVar11 <= (int)uVar23) {
                                            uVar4 = iVar20 * 0x80;
                                            uVar5 = iVar22 * 0x800;
                                            do {
                                                dVar26 = (double)(float)((double)((float)((double)(float)((double)CONCAT44(
                                                                                                                        0x43300000,uVar23 ^ 0x80000000) -
                                                                                                                4503601774854144.0) + dVar38) - (float)param_2) /
                                                                           dVar33);
                                                uVar17 = uVar5 | uVar4 | uVar23;
                                                if (((dVar34 < dVar26) &&
                                                     ((double)(float)((double)(float)(dVar28 + (double)(float)(dVar26 * dVar26)
                                                                                                  ) + dVar27) < dVar24)) &&
                                                    ((int)uVar23 < 0x3e)) {
                                                    getBlockId__11ChunkPrimerFi(param_14,uVar17);
                                                    iVar21 = isDiggable__26UnderwaterLargeCaveFeatureSFi();
                                                    if (iVar21 != 0) {
                                                        if ((!bVar7) && (param_9_00 != (int *)0x0)) {
                                                            (**(code **)(*param_9_00 + 0xbc))(param_9_00,auStack_190);
                                                            iVar21 = getBiomeType__5BiomeCFv();
                                                            if (iVar21 != 10) goto LAB_02a61184;
                                                        }
                                                        bVar7 = true;
                                                        if ((int)uVar23 < 9) {
                                                            setBlockAndData__11ChunkPrimerFiN21(param_14,uVar17,0xb,uVar16);
                                                        }
                                                        else if ((int)uVar23 < 10) {
                                                            dVar26 = (double)nextFloat__6RandomFv(auStack_180);
                                                            if (dVar25 <= dVar26) {
                                                                setBlockAndData__11ChunkPrimerFiN21(param_14,uVar17,0x31,uVar13);
                                                            }
                                                            else {
                                                                setBlockAndData__11ChunkPrimerFiN21(param_14,uVar17,0xd5,uVar12);
                                                            }
                                                        }
                                                        else {
                                                            bVar1 = false;
                                                            if (iVar22 < 0xf) {
                                                                uVar18 = uVar5 + 0x800 | uVar4 | uVar23;
                                                                iVar21 = getBlockId__11ChunkPrimerFi(param_14,uVar18);
                                                                bVar1 = iVar21 == 0;
                                                                if (bVar1) {
                                                                    setBlockAndData__11ChunkPrimerFiN21(param_14,uVar18,8,uVar14);
                                                                }
                                                                if (0 < iVar22) goto LAB_02a61018;
                                                            }
                                                            else {
                                                            LAB_02a61018:
                                                                uVar18 = uVar5 - 0x800 | uVar4 | uVar23;
                                                                iVar21 = getBlockId__11ChunkPrimerFi(param_14,uVar18);
                                                                if (iVar21 == 0) {
                                                                    bVar1 = true;
                                                                    setBlockAndData__11ChunkPrimerFiN21(param_14,uVar18,8,uVar14);
                                                                }
                                                            }
                                                            if (iVar20 < 0xf) {
                                                                uVar18 = uVar5 | uVar4 + 0x80 | uVar23;
                                                                iVar21 = getBlockId__11ChunkPrimerFi(param_14,uVar18);
                                                                if (iVar21 == 0) {
                                                                    bVar1 = true;
                                                                    setBlockAndData__11ChunkPrimerFiN21(param_14,uVar18,8,uVar14);
                                                                }
                                                                if (0 < iVar20) goto LAB_02a610b8;
                                                            }
                                                            else {
                                                            LAB_02a610b8:
                                                                uVar18 = uVar5 | uVar4 - 0x80 | uVar23;
                                                                iVar21 = getBlockId__11ChunkPrimerFi(param_14,uVar18);
                                                                if (iVar21 == 0) {
                                                                    setBlockAndData__11ChunkPrimerFiN21(param_14,uVar18,8,uVar14);
                                                                    goto LAB_02a61128;
                                                                }
                                                            }
                                                            if (!bVar1) {
                                                                setBlockAndData__11ChunkPrimerFiN21(param_14,uVar17,9,uVar15);
                                                            }
                                                        }
                                                    }
                                                }
                                            LAB_02a61128:
                                                iVar19 = iVar19 + -1;
                                                uVar23 = uVar23 - 1;
                                            } while (iVar19 != 0);
                                        }
                                    }
                                    local_1dc = local_1dc + -1;
                                    iVar20 = iVar20 + 1;
                                    dVar24 = dVar29;
                                    dVar25 = dVar30;
                                    dVar34 = dVar31;
                                } while (local_1dc != 0);
                            }
                            local_1e0 = local_1e0 + -1;
                            iVar22 = iVar22 + 1;
                            dVar24 = dVar36;
                            dVar25 = dVar35;
                            dVar34 = dVar37;
                        } while (local_1e0 != 0);
                    }
                    if (bVar8) break;
                }
            }
            local_1ec = local_1ec + -1;
            local_1f0 = local_1f0 + 1;
        } while (local_1ec != 0);
    LAB_02a61184:
        instructionSynchronize();
        return;
    }

     */

    void apply() {}
    /*
    void apply__26UnderwaterLargeCaveFeatureFP5LeveliT2P11ChunkPrimer
                   (int param_1,undefined4 param_2,int param_3,int param_4,undefined4 param_5) {
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
      setSeed__6RandomFL(param_1 + 8,(int)uVar17,(int)((ulonglong)uVar17 >> 0x20),(int)uVar17);
      uVar2 = nextInt__6RandomFv(param_1 + 8);
      iVar16 = (((int)uVar2 >> 1) + (uint)((int)uVar2 < 0 && (uVar2 & 1) != 0)) * 2 + 1;
      uVar2 = nextInt__6RandomFv(param_1 + 8);
      iVar7 = param_3 + -8;
      iVar5 = (((int)uVar2 >> 1) + (uint)((int)uVar2 < 0 && (uVar2 & 1) != 0)) * 2 + 1;
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
                setSeed__6RandomFL(param_1 + 8,(uint)uVar17,
                                   (int)uVar15 >> 0x1f ^ (uint)((ulonglong)uVar17 >> 0x20),
                                   uVar15 ^ (uint)uVar17);
                (**(code **)(*(int *)(param_1 + 0x24) + 0x1c))
                          (param_1,param_2,iVar7,iVar8,param_3,param_4,param_5);
                uVar17 = getSeed__5LevelFv(param_2);
                setSeed__6RandomFL(param_1 + 8,(uint)uVar17,
                                   (int)uVar14 >> 0x1f ^ (uint)((ulonglong)uVar17 >> 0x20),
                                   uVar14 ^ (uint)uVar17);
                (**(code **)(*(int *)(param_1 + 0x24) + 0x1c))
                          (param_1,param_2,iVar7,iVar8 + 1,param_3,param_4,param_5);
                uVar17 = getSeed__5LevelFv(param_2);
                setSeed__6RandomFL(param_1 + 8,(uint)uVar17,
                                   (int)uVar13 >> 0x1f ^ (uint)((ulonglong)uVar17 >> 0x20),
                                   uVar13 ^ (uint)uVar17);
                (**(code **)(*(int *)(param_1 + 0x24) + 0x1c))
                          (param_1,param_2,iVar7,iVar8 + 2,param_3,param_4,param_5);
                uVar17 = getSeed__5LevelFv(param_2);
                setSeed__6RandomFL(param_1 + 8,(uint)uVar17,
                                   (int)uVar12 >> 0x1f ^ (uint)((ulonglong)uVar17 >> 0x20),
                                   uVar12 ^ (uint)uVar17);
                (**(code **)(*(int *)(param_1 + 0x24) + 0x1c))
                          (param_1,param_2,iVar7,iVar8 + 3,param_3,param_4,param_5);
                uVar17 = getSeed__5LevelFv(param_2);
                setSeed__6RandomFL(param_1 + 8,(uint)uVar17,
                                   (int)uVar11 >> 0x1f ^ (uint)((ulonglong)uVar17 >> 0x20),
                                   uVar11 ^ (uint)uVar17);
                (**(code **)(*(int *)(param_1 + 0x24) + 0x1c))
                          (param_1,param_2,iVar7,iVar8 + 4,param_3,param_4,param_5);
                uVar17 = getSeed__5LevelFv(param_2);
                setSeed__6RandomFL(param_1 + 8,(uint)uVar17,
                                   (int)uVar10 >> 0x1f ^ (uint)((ulonglong)uVar17 >> 0x20),
                                   uVar10 ^ (uint)uVar17);
                (**(code **)(*(int *)(param_1 + 0x24) + 0x1c))
                          (param_1,param_2,iVar7,iVar8 + 5,param_3,param_4,param_5);
                uVar17 = getSeed__5LevelFv(param_2);
                setSeed__6RandomFL(param_1 + 8,(uint)uVar17,
                                   (int)uVar9 >> 0x1f ^ (uint)((ulonglong)uVar17 >> 0x20),
                                   uVar9 ^ (uint)uVar17);
                (**(code **)(*(int *)(param_1 + 0x24) + 0x1c))
                          (param_1,param_2,iVar7,iVar8 + 6,param_3,param_4,param_5);
                uVar17 = getSeed__5LevelFv(param_2);
                setSeed__6RandomFL(param_1 + 8,(uint)uVar17,
                                   (int)uVar6 >> 0x1f ^ (uint)((ulonglong)uVar17 >> 0x20),
                                   uVar6 ^ (uint)uVar17);
                (**(code **)(*(int *)(param_1 + 0x24) + 0x1c))
                          (param_1,param_2,iVar7,iVar8 + 7,param_3,param_4,param_5);
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
              if (iVar4 < iVar8) goto LAB_02a60134;
            }
            uVar2 = local_50 + iVar8 * iVar5;
            do {
              uVar17 = getSeed__5LevelFv(param_2);
              setSeed__6RandomFL(param_1 + 8,(uint)uVar17,
                                 (int)uVar2 >> 0x1f ^ (uint)((ulonglong)uVar17 >> 0x20),
                                 uVar2 ^ (uint)uVar17);
              (**(code **)(*(int *)(param_1 + 0x24) + 0x1c))
                        (param_1,param_2,iVar7,iVar8,param_3,param_4,param_5);
              iVar8 = iVar8 + 1;
              uVar2 = uVar2 + iVar5;
            } while (iVar8 <= iVar4);
          }
    LAB_02a60134:
          iVar7 = iVar7 + 1;
          local_50 = local_50 + iVar16;
        } while (iVar7 <= param_3 + 8);
      }
      return;
    }
     */

    MU ND static bool isDiggable(uint32_t param_1) {
        if (param_1 < 0x6e) {
            if (param_1 < 4) return true;

            if (7 < param_1) {
                if (param_1 < 0xe) return true;
                if (param_1 == 0x18) return true;
                if (param_1 == 0x31) return true;
                return false;
            }
        } else {
            if (param_1 == 0x6e) return true;
            if (param_1 == 0x9f) return true;
            if (param_1 == 0xac) return true;
            if (param_1 == 0xb3) return true;
        }
        return false;
    }

    /*

    void addTunnel(int64_t seedModifier, Pos2D chunk, ChunkPrimer* chunkPrimer, DoublePos3D start, float tunnelWidth,
                   float tunnelDirection, float tunnelSlope, int currentTunnelSegment, int maxTunnelSegment,
                   double tunnelHeightMultiplier);

    void addRoom(int64_t seedModifier, Pos2D target, ChunkPrimer* chunkPrimer, DoublePos3D roomStart, RNG& rng);

    void recursiveGenerate(int baseChunkX, int baseChunkZ, int targetX, int targetZ, ChunkPrimer* chunkPrimer) override;
    */

};
