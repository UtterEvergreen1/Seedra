#pragma once
#include "PlacementSettings.hpp"
#include "Template.hpp"


class StructureComponentTemplate : public StructureComponent {
    static const PlacementSettings DEFAULT_PLACE_SETTINGS = PlacementSettings();

protected:
    Template m_template;
    PlacementSettings m_placeSettings;
    Pos3D m_templatePosition;

public:
    StructureComponentTemplate() {
        this->m_placeSettings = DEFAULT_PLACE_SETTINGS.copy().setIgnoreEntities(true).setReplacedBlock(lce::BlocksInit::AIR.getDefaultState());
    }

    explicit StructureComponentTemplate(const PieceType componentType) : StructureComponent() {
        this->m_type = componentType;
        this->m_placeSettings = DEFAULT_PLACE_SETTINGS.copy().setIgnoreEntities(true).setReplacedBlock(lce::BlocksInit::AIR.getDefaultState());
    }

protected:
    void setup(const Template& templateIn, const Pos3D templatePos, const PlacementSettings& settings) {
        this->m_template = templateIn;
        this->setCoordMode(EnumFacing::NORTH);
        this->m_templatePosition = templatePos;
        this->m_placeSettings = settings;
        this->setBoundingBoxFromTemplate();
    }

    virtual void writeStructureToNBT(NBTCompound& tagCompound) {
        tagCompound.insert("TPX", makeInt(this->m_templatePosition.getX()));
        tagCompound.insert("TPY", makeInt(this->m_templatePosition.getY()));
        tagCompound.insert("TPZ", makeInt(this->m_templatePosition.getZ()));
    }

    virtual void readStructureFromNBT(NBTCompound& tagCompound, MU const TemplateManager& manager) {
        this->m_templatePosition = Pos3D(
            tagCompound.getOr<int>("TPX", 0),
            tagCompound.getOr<int>("TPY", 0),
            tagCompound.getOr<int>("TPZ", 0)
        );
    }

public:
    bool addComponentParts(World& worldIn, RNG& rngIn, const BoundingBox structureBB) {
        this->m_placeSettings.setBoundingBox(structureBB);
        this->m_template.addBlocksToWorld(worldIn, this->m_templatePosition, this->m_placeSettings, 18);
        const std::map<Pos3D, std::string> map = this->m_template.getDataBlocks(this->m_templatePosition, this->m_placeSettings);

        for (auto& entry: map) {
            const std::string& s = entry.second;
            this->handleDataMarker(s, entry.first, worldIn, rngIn, structureBB);
        }

        return true;
    }

protected:
    virtual void handleDataMarker(std::string marker, Pos3D pos, World& world, RNG& random, BoundingBox structureBB);

private:
    void setBoundingBoxFromTemplate() {
        const Rotation rotation = this->m_placeSettings.getRotation();
        const Pos3D blockPos = this->m_template.transformedSize(rotation);
        const Mirror mirror = this->m_placeSettings.getMirror();
        this->setBoundingBox(BoundingBox(
            0,
            0,
            0,
            static_cast<bbType_t>(blockPos.getX()),
            static_cast<bbType_t>(blockPos.getY() - 1),
            static_cast<bbType_t>(blockPos.getZ())
        ));

        switch (rotation.type) {
            case Rotation::Type::NONE:
            default:
                break;

            case Rotation::Type::CLOCKWISE_90:
                this->offset(-blockPos.getX(), 0, 0);
                break;

            case Rotation::Type::COUNTERCLOCKWISE_90:
                this->offset(0, 0, -blockPos.getZ());
                break;

            case Rotation::Type::CLOCKWISE_180:
                this->offset(-blockPos.getX(), 0, -blockPos.getZ());
        }

        switch (mirror.type) {
            case Mirror::Type::NONE:
            default:
                break;

            case Mirror::Type::FRONT_BACK: {

                Pos3D blockPos2 = {0, 0, 0}; // Pos3D.ORIGIN;

                if (rotation != Rotation::CLOCKWISE_90 && rotation != Rotation::COUNTERCLOCKWISE_90) {
                    if (rotation == Rotation::CLOCKWISE_180) {
                        blockPos2 = blockPos2.offset(EnumFacing::EAST, blockPos.getX());
                    } else {
                        blockPos2 = blockPos2.offset(EnumFacing::WEST, blockPos.getX());
                    }
                } else {
                    blockPos2 = blockPos2.offset(rotation.rotateFacing(EnumFacing::WEST), blockPos.getZ());
                }

                this->offset(blockPos2.getX(), 0, blockPos2.getZ());
                break;
            }

            case Mirror::Type::LEFT_RIGHT: {

                Pos3D blockPos1 = {0, 0, 0}; // Pos3D.ORIGIN;

                if (rotation != Rotation::CLOCKWISE_90 && rotation != Rotation::COUNTERCLOCKWISE_90) {
                    if (rotation == Rotation::CLOCKWISE_180) {
                        blockPos1 = blockPos1.offset(EnumFacing::SOUTH, blockPos.getZ());
                    } else {
                        blockPos1 = blockPos1.offset(EnumFacing::NORTH, blockPos.getZ());
                    }
                } else {
                    blockPos1 = blockPos1.offset(rotation.rotateFacing(EnumFacing::NORTH), blockPos.getX());
                }

                this->offset(blockPos1.getX(), 0, blockPos1.getZ());
            }
        }

        this->offset(this->m_templatePosition.getX(), this->m_templatePosition.getY(),
                     this->m_templatePosition.getZ());
    }

public:
    void offset(const int x, const int y, const int z) {
        this->BoundingBox::offset(x, y, z);
        this->m_templatePosition = this->m_templatePosition.add(x, y, z);
    }
};
