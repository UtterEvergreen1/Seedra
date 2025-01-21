#include "WorldGenBigTree.hpp"

#include <cmath>

#include "lce/blocks/__include.hpp"
#include "terrain/World.hpp"


using namespace lce::blocks;


bool WorldGenBigTree::generate(World* worldIn, RNG& rng, const Pos3D& position) const {
    this->world = worldIn;
    this->basePos = position;
    this->rand = RNG(rng.nextLong());

    this->heightLimit = 5 + rng.nextInt(this->heightLimitLimit);

    if (!this->validTreeLocation()) { return false; }

    this->generateLeafNodeList();
    this->generateLeaves();
    this->generateTrunk();
    this->generateLeafNodeBases();
    return true;
}

void WorldGenBigTree::generateLeafNodeList() const {
    this->height = static_cast<int>(this->heightLimit * this->heightAttenuation);

    if (this->height >= this->heightLimit) { this->height = this->heightLimit - 1; }

    int i = static_cast<int>(1.382 + std::pow(this->leafDensity * static_cast<double>(this->heightLimit) / 13.0, 2.0));

    if (i < 1) { i = 1; }

    int j = this->basePos.getY() + this->height;
    int k = this->heightLimit - this->leafDistanceLimit;
    this->foliageCoords.clear();
    this->foliageCoords.emplace_back(this->basePos.up(k), j);

    for (; k >= 0; --k) {
        const float f = this->layerSize(k);

        if (f >= 0.0F) {
            for (int l = 0; l < i; ++l) {
                c_double d0 = this->scaleWidth * (double) f * ((double) this->rand.nextFloat() + 0.328);
                c_double d1 = (double) (this->rand.nextFloat() * 2.0F) * M_PI;
                c_double d2 = d0 * std::sin(d1) + 0.5;
                c_double d3 = d0 * std::cos(d1) + 0.5;
                Pos3D blockPos = this->basePos.add((int) std::floor(d2), k - 1, (int) std::floor(d3));
                Pos3D blockPos1 = blockPos.up(this->leafDistanceLimit);

                if (this->checkBlockLine(blockPos, blockPos1) == -1) {
                    c_int i1 = this->basePos.getX() - blockPos.getX();
                    c_int j1 = this->basePos.getZ() - blockPos.getZ();
                    c_double d4 = (double) blockPos.getY() - std::sqrt((double) (i1 * i1 + j1 * j1)) * this->branchSlope;
                    c_int k1 = d4 > (double) j ? j : static_cast<int>(d4);
                    Pos3D blockPos2(this->basePos.getX(), k1, this->basePos.getZ());

                    if (this->checkBlockLine(blockPos2, blockPos) == -1) {
                        this->foliageCoords.emplace_back(blockPos, blockPos2.getY());
                    }
                }
            }
        }
    }
}

void WorldGenBigTree::crossSection(const Pos3D& pos, const float radius, const lce::Block* block) const {
    c_int i = static_cast<int>(radius + 0.618);

    for (int j = -i; j <= i; ++j) {
        for (int k = -i; k <= i; ++k) {
            if (std::pow(std::abs(j) + 0.5, 2.0) + std::pow(std::abs(k) + 0.5, 2.0) <= radius * radius) {
                Pos3D blockPos = pos.add(j, 0, k);
                c_int material = this->world->getBlockId(blockPos);

                if (lce::blocks::isAirOrLeavesBlock(material)) {
                    this->world->setBlock(blockPos, block);
                }
            }
        }
    }
}

float WorldGenBigTree::layerSize(c_int y) const {
    if (y < this->heightLimit * 0.3) { return -1.0F; }

    float f = static_cast<float>(this->heightLimit) / 2.0F;
    const float f1 = f - static_cast<float>(y);
    float f2 = std::sqrt(f * f - f1 * f1);

    if (f1 == 0.0F) {
        f2 = f;
    } else if (std::abs(f1) >= f) {
        return 0.0F;
    }

    return f2 * 0.5F;
}

float WorldGenBigTree::leafSize(c_int y) const {
    if (y >= 0 && y < this->leafDistanceLimit) {
        return y != 0 && y != this->leafDistanceLimit - 1 ? 3.0F : 2.0F;
    } else {
        return -1.0F;
    }
}

void WorldGenBigTree::generateLeafNode(const Pos3D& pos) const {
    for (int i = 0; i < this->leafDistanceLimit; ++i) {
        this->crossSection(pos.up(i), this->leafSize(i), &lce::BlocksInit::OAK_LEAVES);
    }
}

void WorldGenBigTree::limb(const Pos3D& start, const Pos3D& end, const lce::Block* block) const {
    const Pos3D blockPos = end.add(-start.getX(), -start.getY(), -start.getZ());
    c_int i = WorldGenBigTree::getGreatestDistance(blockPos);
    const float f = static_cast<float>(blockPos.getX()) / static_cast<float>(i);
    const float f1 = static_cast<float>(blockPos.getY()) / static_cast<float>(i);
    const float f2 = static_cast<float>(blockPos.getZ()) / static_cast<float>(i);

    for (int j = 0; j <= i; ++j) {
        Pos3D blockPos1 =
                start.add((int) (0.5F + (float) j * f), (int) (0.5F + (float) j * f1), (int) (0.5F + (float) j * f2));
        const EnumAxis axis = getLogAxis(start, blockPos1);
        c_int metaStateAxis = axis == EnumAxis::Y ? 0 : axis == EnumAxis::X ? 4 : 8;
        this->world->setBlock(blockPos1, block->getID(), block->getDataTag() | metaStateAxis);
    }
}

int WorldGenBigTree::getGreatestDistance(const Pos3D& pos) {
    int i = std::abs(pos.getX());
    int j = std::abs(pos.getY());
    int k = std::abs(pos.getZ());

    if (k > i && k > j) { return k; }

    return j > i ? j : i;
}

EnumAxis WorldGenBigTree::getLogAxis(const Pos3D& start, const Pos3D& end) {
    auto axis = EnumAxis::Y;
    int i = std::abs(end.getX() - start.getX());
    int j = std::abs(end.getZ() - start.getZ());
    int k = std::max(i, j);

    if (k > 0) {
        if (i == k) {
            axis = EnumAxis::X;
        } else if (j == k) {
            axis = EnumAxis::Z;
        }
    }

    return axis;
}

void WorldGenBigTree::generateLeaves() const {
    for (const Pos3D& foliage: this->foliageCoords) { this->generateLeafNode(foliage); }
}

bool WorldGenBigTree::leafNodeNeedsBase(c_int y) const { return static_cast<double>(y) >= this->heightLimit * 0.2; }

void WorldGenBigTree::generateTrunk() const {
    const Pos3D blockPos = this->basePos;
    const Pos3D blockPos1 = this->basePos.up(this->height);
    const lce::Block* trunkBlock = &lce::BlocksInit::OAK_WOOD;
    this->limb(blockPos, blockPos1, trunkBlock);

    if (this->trunkSize == 2) {
        this->limb(blockPos.east(), blockPos1.east(), trunkBlock);
        this->limb(blockPos.east().south(), blockPos1.east().south(), trunkBlock);
        this->limb(blockPos.south(), blockPos1.south(), trunkBlock);
    }
}

void WorldGenBigTree::generateLeafNodeBases() const {
    for (const Pos3D& foliage: this->foliageCoords) {
        c_int i = foliage.getY();
        const Pos3D blockPos(this->basePos.getX(), i, this->basePos.getZ());

        if (blockPos != foliage && this->leafNodeNeedsBase(i - this->basePos.getY())) {
            this->limb(blockPos, foliage, &lce::BlocksInit::OAK_WOOD);
        }
    }
}

int WorldGenBigTree::checkBlockLine(const Pos3D& start, const Pos3D& end) const {
    const Pos3D blockPos = end.add(-start.getX(), -start.getY(), -start.getZ());
    c_int i = getGreatestDistance(blockPos);
    const float f = static_cast<float>(blockPos.getX()) / static_cast<float>(i);
    const float f1 = static_cast<float>(blockPos.getY()) / static_cast<float>(i);
    const float f2 = static_cast<float>(blockPos.getZ()) / static_cast<float>(i);

    if (i == 0) {
        return -1;
    } else {
        for (int j = 0; j <= i; ++j) {
            Pos3D blockPos1 = start.add((int) (0.5F + (float) j * f),
                                        (int) (0.5F + (float) j * f1),
                                        (int) (0.5F + (float) j * f2));

            if (!canGrowInto(this->world->getBlockId(blockPos1))) { return j; }
        }

        return -1;
    }
}

bool WorldGenBigTree::validTreeLocation() const {
    c_int block = this->world->getBlockId(this->basePos.down());

    if (!lce::blocks::isGrassOrDirtOrFarmland(block)) {
        return false;
    }

    c_int i = this->checkBlockLine(this->basePos, this->basePos.up(this->heightLimit - 1));

    if (i == -1) return true;

    if (i < 6) return false;

    this->heightLimit = i;
    return true;
}
