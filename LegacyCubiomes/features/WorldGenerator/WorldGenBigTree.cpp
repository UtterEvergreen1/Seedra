#include "WorldGenBigTree.hpp"

#include "LegacyCubiomes/chunk_generator/World.hpp"
#include "lce/blocks/blocks.hpp"
#include "lce/blocks/block_ids.hpp"

#include <cmath>


bool WorldGenBigTree::generate(World* worldIn, RNG& rand, const Pos3D& position) const {
    this->world = worldIn;
    this->basePos = position;
    this->rand = RNG(rand.nextLong());

    this->heightLimit = 5 + rand.nextInt(this->heightLimitLimit);

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
                const double d0 = this->scaleWidth * (double) f * ((double) this->rand.nextFloat() + 0.328);
                const double d1 = (double) (this->rand.nextFloat() * 2.0F) * M_PI;
                const double d2 = d0 * std::sin(d1) + 0.5;
                const double d3 = d0 * std::cos(d1) + 0.5;
                Pos3D blockpos = this->basePos.add((int) std::floor(d2), k - 1, (int) std::floor(d3));
                Pos3D blockpos1 = blockpos.up(this->leafDistanceLimit);

                if (this->checkBlockLine(blockpos, blockpos1) == -1) {
                    const int i1 = this->basePos.getX() - blockpos.getX();
                    const int j1 = this->basePos.getZ() - blockpos.getZ();
                    const double d4 = (double) blockpos.getY() - std::sqrt((double) (i1 * i1 + j1 * j1)) * this->branchSlope;
                    const int k1 = d4 > (double) j ? j : static_cast<int>(d4);
                    Pos3D blockpos2(this->basePos.getX(), k1, this->basePos.getZ());

                    if (this->checkBlockLine(blockpos2, blockpos) == -1) {
                        this->foliageCoords.emplace_back(blockpos, blockpos2.getY());
                    }
                }
            }
        }
    }
}

void WorldGenBigTree::crossSection(const Pos3D& pos, const float radius, const lce::blocks::Block* block) const {
    const int i = static_cast<int>(radius + 0.618);

    for (int j = -i; j <= i; ++j) {
        for (int k = -i; k <= i; ++k) {
            if (std::pow(std::abs(j) + 0.5, 2.0) + std::pow(std::abs(k) + 0.5, 2.0) <= radius * radius) {
                Pos3D blockpos = pos.add(j, 0, k);
                const int material = this->world->getBlockId(blockpos);

                if (lce::blocks::ids::isAirOrLeavesBlock(material)) {
                    this->world->setBlock(blockpos, block);
                }
            }
        }
    }
}

float WorldGenBigTree::layerSize(const int y) const {
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

float WorldGenBigTree::leafSize(const int y) const {
    if (y >= 0 && y < this->leafDistanceLimit) {
        return y != 0 && y != this->leafDistanceLimit - 1 ? 3.0F : 2.0F;
    } else {
        return -1.0F;
    }
}

void WorldGenBigTree::generateLeafNode(const Pos3D& pos) const {
    for (int i = 0; i < this->leafDistanceLimit; ++i) {
        this->crossSection(pos.up(i), this->leafSize(i), &lce::blocks::BlocksInit::OAK_LEAVES);
    }
}

void WorldGenBigTree::limb(const Pos3D& start, const Pos3D& end, const lce::blocks::Block* block) const {
    const Pos3D blockpos = end.add(-start.getX(), -start.getY(), -start.getZ());
    const int i = WorldGenBigTree::getGreatestDistance(blockpos);
    const float f = static_cast<float>(blockpos.getX()) / static_cast<float>(i);
    const float f1 = static_cast<float>(blockpos.getY()) / static_cast<float>(i);
    const float f2 = static_cast<float>(blockpos.getZ()) / static_cast<float>(i);

    for (int j = 0; j <= i; ++j) {
        Pos3D blockpos1 =
                start.add((int) (0.5F + (float) j * f), (int) (0.5F + (float) j * f1), (int) (0.5F + (float) j * f2));
        const EnumAxis axis = getLogAxis(start, blockpos1);
        const int metaStateAxis = axis == EnumAxis::Y ? 0 : axis == EnumAxis::X ? 4 : 8;
        this->world->setBlock(blockpos1, block->getID(), block->getDataTag() | metaStateAxis);
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

bool WorldGenBigTree::leafNodeNeedsBase(const int y) const { return static_cast<double>(y) >= this->heightLimit * 0.2; }

void WorldGenBigTree::generateTrunk() const {
    const Pos3D blockpos = this->basePos;
    const Pos3D blockpos1 = this->basePos.up(this->height);
    const lce::blocks::Block* trunkBlock = &lce::blocks::BlocksInit::OAK_WOOD;
    this->limb(blockpos, blockpos1, trunkBlock);

    if (this->trunkSize == 2) {
        this->limb(blockpos.east(), blockpos1.east(), trunkBlock);
        this->limb(blockpos.east().south(), blockpos1.east().south(), trunkBlock);
        this->limb(blockpos.south(), blockpos1.south(), trunkBlock);
    }
}

void WorldGenBigTree::generateLeafNodeBases() const {
    for (const Pos3D& foliage: this->foliageCoords) {
        const int i = foliage.getY();
        const Pos3D blockPos(this->basePos.getX(), i, this->basePos.getZ());

        if (blockPos != foliage && this->leafNodeNeedsBase(i - this->basePos.getY())) {
            this->limb(blockPos, foliage, &lce::blocks::BlocksInit::OAK_WOOD);
        }
    }
}

int WorldGenBigTree::checkBlockLine(const Pos3D& start, const Pos3D& end) const {
    const Pos3D blockpos = end.add(-start.getX(), -start.getY(), -start.getZ());
    const int i = getGreatestDistance(blockpos);
    const float f = static_cast<float>(blockpos.getX()) / static_cast<float>(i);
    const float f1 = static_cast<float>(blockpos.getY()) / static_cast<float>(i);
    const float f2 = static_cast<float>(blockpos.getZ()) / static_cast<float>(i);

    if (i == 0) {
        return -1;
    } else {
        for (int j = 0; j <= i; ++j) {
            Pos3D blockpos1 = start.add((int) (0.5F + (float) j * f), (int) (0.5F + (float) j * f1),
                                        (int) (0.5F + (float) j * f2));

            if (!canGrowInto(this->world->getBlockId(blockpos1))) { return j; }
        }

        return -1;
    }
}

bool WorldGenBigTree::validTreeLocation() const {
    const int block = this->world->getBlockId(this->basePos.down());

    if (!lce::blocks::ids::isGrassOrDirtOrFarmland(block)) {
        return false;
    }

    const int i = this->checkBlockLine(this->basePos, this->basePos.up(this->heightLimit - 1));

    if (i == -1) return true;

    if (i < 6) return false;

    this->heightLimit = i;
    return true;
}
