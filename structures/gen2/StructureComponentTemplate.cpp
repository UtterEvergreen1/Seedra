#include "StructureComponentTemplate.hpp"

#include "Template.hpp"
#include "TemplateManager.hpp"
#include "terrain/World.hpp"

const PlacementSettings StructureComponentTemplate::DEFAULT_PLACE_SETTINGS = PlacementSettings();


StructureComponentTemplate::StructureComponentTemplate() {
    this->m_placeSettings = DEFAULT_PLACE_SETTINGS.copy().setIgnoreEntities(true).setReplacedBlock(lce::BlocksInit::AIR.getDefaultState());
}


StructureComponentTemplate::StructureComponentTemplate(const PieceType componentType) : StructureComponent() {
    this->m_type = componentType;
    this->m_placeSettings = DEFAULT_PLACE_SETTINGS.copy().setIgnoreEntities(true).setReplacedBlock(lce::BlocksInit::AIR.getDefaultState());
}


void StructureComponentTemplate::setup(Template* templateIn, const Pos3D templatePos,
                                       const PlacementSettings& settings) {
    this->m_template = templateIn;
    this->setCoordMode(EnumFacing::NORTH);
    this->m_templatePosition = templatePos;
    this->m_placeSettings = settings;
    this->setBoundingBoxFromTemplate();
}


MU void StructureComponentTemplate::writeStructureToNBT(NBTCompound& tagCompound) {
    tagCompound.insert("TPX", makeInt(this->m_templatePosition.getX()));
    tagCompound.insert("TPY", makeInt(this->m_templatePosition.getY()));
    tagCompound.insert("TPZ", makeInt(this->m_templatePosition.getZ()));
}


MU void StructureComponentTemplate::readStructureFromNBT(NBTCompound& tagCompound, const TemplateManager& manager) {
    this->m_templatePosition = Pos3D(
            tagCompound.getOr<int>("TPX", 0),
            tagCompound.getOr<int>("TPY", 0),
            tagCompound.getOr<int>("TPZ", 0)
    );
}


bool StructureComponentTemplate::addComponentParts(
        World& worldIn, RNG& rngIn, const BoundingBox structureBB
) {
#if 0 // <-- flip to 0 to restore normal template placement
    // World-space BB of this piece (already offset in setBoundingBoxFromTemplate()).
    BoundingBox pieceBB = *static_cast<BoundingBox*>(this);  // or `this->m_boundingBox`

    // Clamp to current chunk bounds.
    BoundingBox fillBB = pieceBB;

    fillBB.shrinkToFit(structureBB);

    // Bail if nothing overlaps this chunk.
    // If you don't have getters, see notes below.
    if (fillBB.m_minX > fillBB.m_maxX ||
        fillBB.m_minY > fillBB.m_maxY ||
        fillBB.m_minZ > fillBB.m_maxZ) {
        return true;
    }

    const auto stone = lce::BlocksInit::STONE.getDefaultState();

    for (int y = fillBB.m_minY; y <= fillBB.m_maxY; ++y) {
        for (int z = fillBB.m_minZ; z <= fillBB.m_maxZ; ++z) {
            for (int x = fillBB.m_minX; x <= fillBB.m_maxX; ++x) {
                worldIn.setBlock(Pos3D(x, y, z),  lce::BlocksInit::STONE.getDefaultState());
                // or: worldIn.setBlockId(Pos3D(x,y,z), lce::blocks::STONE_ID);
            }
        }
    }

    return true;
#else
    // Normal behavior
    this->m_placeSettings.setBoundingBox(structureBB);
    this->m_template->addBlocksToWorld(worldIn, this->m_templatePosition, this->m_placeSettings, 18);

    const std::map<Pos3D, std::string> map =
            this->m_template->getDataBlocks(this->m_templatePosition, this->m_placeSettings);

    for (auto& entry: map) {
        this->handleDataMarker(entry.second, entry.first, worldIn, rngIn, structureBB);
    }
    return true;
#endif
}


void StructureComponentTemplate::setBoundingBoxFromTemplate() {
    const Rotation rotation = this->m_placeSettings.getRotation();
    const Pos3D blockPos = this->m_template->transformedSize(rotation);
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
            BoundingBox::offset(-blockPos.getX(), 0, 0);
            break;

        case Rotation::Type::COUNTERCLOCKWISE_90:
            BoundingBox::offset(0, 0, -blockPos.getZ());
            break;

        case Rotation::Type::CLOCKWISE_180:
            BoundingBox::offset(-blockPos.getX(), 0, -blockPos.getZ());
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

            BoundingBox::offset(blockPos2.getX(), 0, blockPos2.getZ());
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

            BoundingBox::offset(blockPos1.getX(), 0, blockPos1.getZ());
        }
    }

    BoundingBox::offset(this->m_templatePosition.getX(), this->m_templatePosition.getY(),
                 this->m_templatePosition.getZ());
}
