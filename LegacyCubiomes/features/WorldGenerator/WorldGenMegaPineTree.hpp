#pragma once

#include "WorldGenHugeTrees.hpp"


class World;

class WorldGenMegaPineTree final : public WorldGenHugeTrees {
    bool useBaseHeight;

public:
    explicit WorldGenMegaPineTree(bool useBaseHeight);

    bool generate(World* worldIn, RNG& rand, const Pos3D& position) const override;

    void generateSaplings(World* worldIn, RNG& random, const Pos3D& pos) const override;

private:
    void createCrown(World* worldIn, int x, int z, int y, int p_150541_5_, RNG& rand) const;

    static void placePodzolCircle(World* worldIn, const Pos3D& center);

    static void placePodzolAt(World* worldIn, const Pos3D& pos);
};
