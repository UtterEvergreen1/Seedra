#pragma once

#include "PlacementSettings.hpp"
#include "components/PieceType.hpp"
#include "components/StructureComponent.hpp"


class Template;
class TemplateManager;
class NBTCompound;
class World;

class StructureComponentTemplate : public StructureComponent {
    static const PlacementSettings DEFAULT_PLACE_SETTINGS;

protected:
    Template* m_template{};
    PlacementSettings m_placeSettings;
    Pos3D m_templatePosition;

public:
    StructureComponentTemplate();

    explicit StructureComponentTemplate(PieceType componentType);

protected:
    void setup(Template* templateIn, Pos3D templatePos, const PlacementSettings& settings);

    virtual void writeStructureToNBT(NBTCompound& tagCompound);

    virtual void readStructureFromNBT(NBTCompound& tagCompound, MU const TemplateManager& manager);

public:
    bool addComponentParts(World& worldIn, RNG& rngIn, BoundingBox structureBB);

protected:
    virtual void handleDataMarker(std::string marker, Pos3D pos, World& world, RNG& random, BoundingBox structureBB) {}

private:
    void setBoundingBoxFromTemplate();

public:
    void offset(const int x, const int y, const int z) {
        this->BoundingBox::offset(x, y, z);
        this->m_templatePosition = this->m_templatePosition.add(x, y, z);
    }
};
