#pragma once

#include "common/RotationAndMirror.hpp"
#include "common/rng.hpp"
#include "components/BoundingBox.hpp"
#include "lce/blocks/block.hpp"


class PlacementSettings {
    Mirror m_mirror = Mirror::NONE;
    Rotation m_rotation = Rotation::NONE;
    bool m_ignoreEntities = false;
    std::optional<lce::BlockState> m_replacedBlock;
    std::optional<Pos2D> m_chunkPos;
    std::optional<BoundingBox> m_boundingBox;
    bool m_ignoreStructureBlock = true;
    float m_integrity = 1.0F;
    std::optional<RNG> m_rng;
    std::optional<int64_t> m_seed;

public:
    ND PlacementSettings copy() const {
        PlacementSettings ps;
        ps.m_mirror = this->m_mirror;
        ps.m_rotation = this->m_rotation;
        ps.m_ignoreEntities = this->m_ignoreEntities;
        ps.m_replacedBlock = this->m_replacedBlock;
        ps.m_chunkPos = this->m_chunkPos;
        ps.m_boundingBox = this->m_boundingBox;
        ps.m_ignoreStructureBlock = this->m_ignoreStructureBlock;
        ps.m_integrity = this->m_integrity;
        ps.m_rng = this->m_rng;
        ps.m_seed = this->m_seed;
        return ps;
    }

    PlacementSettings& setMirror(const Mirror mirror) {
        this->m_mirror = mirror;
        return *this;
    }

    PlacementSettings& setRotation(const Rotation rotation) {
        this->m_rotation = rotation;
        return *this;
    }

    PlacementSettings& setIgnoreEntities(const bool ignoreEntities) {
        this->m_ignoreEntities = ignoreEntities;
        return *this;
    }

    PlacementSettings& setReplacedBlock(const lce::BlockState blockState) {
        this->m_replacedBlock = blockState;
        return *this;
    }

    MU PlacementSettings& setChunkPos(const Pos2D chunkPos) {
        this->m_chunkPos = chunkPos;
        return *this;
    }

    PlacementSettings& setBoundingBox(const BoundingBox& structureBB) {
        this->m_boundingBox = structureBB;
        return *this;
    }

    PlacementSettings& setSeed(const std::optional<int64_t> seed) {
        this->m_seed = seed;
        return *this;
    }

    MU PlacementSettings& setRNG(const std::optional<RNG>& rng) {
        this->m_rng = rng;
        return *this;
    }

    MU PlacementSettings& setIntegrity(const float integrity) {
        this->m_integrity = integrity;
        return *this;
    }

    ND Mirror getMirror() const { return this->m_mirror; }

    MU PlacementSettings& setIgnoreStructureBlock(const bool ignoreStructureBlock) {
        this->m_ignoreStructureBlock = ignoreStructureBlock;
        return *this;
    }

    ND Rotation getRotation() const { return this->m_rotation; }

    MU ND RNG getRandom(const std::optional<Pos3D>& posIn) const {
        if (this->m_rng.has_value()) {
            return this->m_rng.value();
        }
        if (this->m_seed.has_value()) {
            return this->m_seed.value() == 0L ? RNG(0/*System.currentTimeMillis()*/)
                                               : RNG(this->m_seed.value());
        }
        if (!posIn.has_value()) {
            return RNG(0/*System.currentTimeMillis()*/);
        }
        const int64_t i = posIn.value().getX();
        const int64_t j = posIn.value().getZ();
        return RNG(
            (i * i * 4987142LL + i * 5947611LL) +
            (j * j) * 4392871LL +
            (j * 389711LL) ^ 987234911LL
        );
    }

    MU ND float getIntegrity() const { return this->m_integrity; }

    ND bool getIgnoreEntities() const { return this->m_ignoreEntities; }

    ND std::optional<lce::BlockState> getReplacedBlock() const { return this->m_replacedBlock; }

    ND std::optional<BoundingBox> getBoundingBox() {
        if (!this->m_boundingBox.has_value() && this->m_chunkPos.has_value()) {
            this->setBoundingBoxFromChunk();
        }
        return this->m_boundingBox;
    }

    ND bool getIgnoreStructureBlock() const { return this->m_ignoreStructureBlock; }

    void setBoundingBoxFromChunk() {
        this->m_boundingBox = createChunkBoundingBox(this->m_chunkPos);
    }
private:

    static std::optional<BoundingBox> createChunkBoundingBox(const std::optional<Pos2D>& chunkPos) {
        if (!chunkPos.has_value()) {
            return std::nullopt;
        }
        const int i = chunkPos->x * 16;
        const int j = chunkPos->z * 16;
        return BoundingBox(
            static_cast<bbType_t>(i),
            0,
            static_cast<bbType_t>(j),
            static_cast<bbType_t>(i + 16 - 1),
            255,
            static_cast<bbType_t>(j + 16 - 1)
        );
    }
};