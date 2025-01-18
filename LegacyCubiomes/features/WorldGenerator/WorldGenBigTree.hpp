#pragma once

#include "WorldGenAbstractTree.hpp"


namespace lce::blocks { class Block; }
class World;

class WorldGenBigTree final : public WorldGenAbstractTree {
    class FoliageCoordinates;
    mutable RNG rand;
    mutable World* world{};
    mutable Pos3D basePos;
    mutable int heightLimit{};
    mutable int height{};
    const double heightAttenuation = 0.618;
    const double branchSlope = 0.381;
    const double scaleWidth = 1.0;
    const double leafDensity = 1.0;
    int trunkSize = 1;
    int heightLimitLimit = 12;
    mutable int leafDistanceLimit = 4;
    mutable std::vector<FoliageCoordinates> foliageCoords;

public:
    bool generate(World* worldIn, RNG& rng, const Pos3D& position) const override;

    void setDecorationDefaults() const override { this->leafDistanceLimit = 5; }

private:
    void generateLeafNodeList() const;
    void crossSection(const Pos3D& pos, float radius, const lce::blocks::Block* block) const;
    float layerSize(int y) const;
    float leafSize(int y) const;
    void generateLeafNode(const Pos3D& pos) const;
    void limb(const Pos3D& start, const Pos3D& end, const lce::blocks::Block* block) const;
    static int getGreatestDistance(const Pos3D& pos);
    static EnumAxis getLogAxis(const Pos3D& start, const Pos3D& end);
    void generateLeaves() const;
    bool leafNodeNeedsBase(int y) const;
    void generateTrunk() const;
    void generateLeafNodeBases() const;
    int checkBlockLine(const Pos3D& start, const Pos3D& end) const;
    bool validTreeLocation() const;

    class FoliageCoordinates : public Pos3D {
    public:
        FoliageCoordinates(const Pos3D& pos, const int branch_base) : Pos3D(pos), branchBase(branch_base) {}

        ND int getBranchBase() const { return this->branchBase; }

    private:
        int branchBase;
    };
};