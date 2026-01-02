#pragma once

#include <array>
#include <list>
#include <string>
#include <unordered_map>
#include <utility>

#include "common/RotationAndMirror.hpp"
#include "common/rng.hpp"

#include "Seedra/structures/gen2/ResourceLocation.hpp"
#include "Seedra/structures/gen2/StructureComponentTemplate.hpp"
#include "Seedra/structures/gen2/Template.hpp"
#include "Seedra/structures/gen2/TemplateManager.hpp"
#include "Seedra/terrain/World.hpp"

template<typename T>
void Collections_shuffle(std::vector<T>& vec, RNG& rnd) {
    std::size_t size = vec.size();
    for (std::size_t i = size; i > 1; --i) {
        auto j = static_cast<std::size_t>(rnd.nextInt(static_cast<int>(i)));
        std::swap(vec[i - 1], vec[j]);
    }
}


class MU OceanMonumentPieces {
public:
    class RoomDefinition {
    public:
        int index = -1; // -1 means "no room"
        std::array<RoomDefinition*, 6> connections = {};
        std::array<bool, 6> hasOpening = {};
        bool claimed{};
        bool isSource{};
        int scanIndex{};

        explicit constexpr RoomDefinition() = default;

        explicit constexpr RoomDefinition(int indexIn) : index(indexIn) {}

        void setConnection(EnumFacing facing, RoomDefinition& definition) {
            this->connections[getIndexFacing(facing)] = &definition;
            definition.connections[getIndexFacing(getOppositeFacing(facing))] = this;
        }

        void updateOpenings() {
            for (int i = 0; i < 6; ++i) { this->hasOpening[i] = this->connections[i] != nullptr; }
        }

        bool findSource(int p_175959_1_) {
            if (this->isSource) {
                return true;
            } else {
                this->scanIndex = p_175959_1_;

                for (int i = 0; i < 6; ++i) {
                    if (this->connections[i] != nullptr && this->hasOpening[i] &&
                        this->connections[i]->scanIndex != p_175959_1_ &&
                        this->connections[i]->findSource(p_175959_1_)) {
                        return true;
                    }
                }

                return false;
            }
        }

        ND bool isSpecial() const { return this->index >= 75; }

        int countOpenings() {
            int i = 0;

            for (int j = 0; j < 6; ++j) {
                if (this->hasOpening[j]) { ++i; }
            }

            return i;
        }
    };

    class OceanMonumentPiece : public StructureComponent {
    public:
        static constexpr int getRoomIndex(int p_175820_0_, int p_175820_1_, int p_175820_2_) {
            return p_175820_1_ * 25 + p_175820_2_ * 5 + p_175820_0_;
        }

        static constexpr lce::BlockState ROUGH_PRISMARINE = lce::BlocksInit::PRISMARINE.getStateFromMeta(0);
        static constexpr lce::BlockState BRICKS_PRISMARINE = lce::BlocksInit::PRISMARINE.getStateFromMeta(1);
        static constexpr lce::BlockState DARK_PRISMARINE = lce::BlocksInit::PRISMARINE.getStateFromMeta(2);
        static constexpr lce::BlockState DOT_DECO_DATA = BRICKS_PRISMARINE;
        static constexpr lce::BlockState SEA_LANTERN = lce::BlocksInit::SEA_LANTERN.getDefaultState();
        static constexpr lce::BlockState WATER = lce::BlocksInit::STILL_WATER.getDefaultState();
        static int GRIDROOM_SOURCE_INDEX;
        static int GRIDROOM_TOP_CONNECT_INDEX;
        static int GRIDROOM_LEFTWING_CONNECT_INDEX;
        static int GRIDROOM_RIGHTWING_CONNECT_INDEX;

        static RoomDefinition NON_INITIALIZED_ROOM_DEF;
        RoomDefinition* roomDefinition = &NON_INITIALIZED_ROOM_DEF;


    public:
        OceanMonumentPiece() : StructureComponent() {}

        explicit OceanMonumentPiece(int pieceType) : StructureComponent() {
            this->m_type = static_cast<PieceType>(pieceType);
        }

        OceanMonumentPiece(EnumFacing facing, BoundingBox bbIn) : StructureComponent() {
            this->m_type = static_cast<PieceType>(1);
            this->setCoordMode(facing);
            this->setBoundingBox(bbIn);
        }

        OceanMonumentPiece(int pieceType, EnumFacing facing, RoomDefinition* definition, int p_i45590_4_,
                           int p_i45590_5_, int p_i45590_6_) : StructureComponent() {
            this->m_type = static_cast<PieceType>(pieceType);
            this->setCoordMode(facing);
            this->roomDefinition = definition;
            int i = definition->index;
            int j = i % 5;
            int k = i / 5 % 5;
            int l = i / 25;

            if (facing != EnumFacing::NORTH && facing != EnumFacing::SOUTH) {
                this->setBoundingBox(BoundingBox(0, 0, 0,
                                                 static_cast<bbType_t>(p_i45590_6_ * 8 - 1),
                                                 static_cast<bbType_t>(p_i45590_5_ * 4 - 1),
                                                 static_cast<bbType_t>(p_i45590_4_ * 8 - 1)
                                                 ));
            } else {
                this->setBoundingBox(BoundingBox(0, 0, 0,
                                                 static_cast<bbType_t>(p_i45590_4_ * 8 - 1),
                                                 static_cast<bbType_t>(p_i45590_5_ * 4 - 1),
                                                 static_cast<bbType_t>(p_i45590_6_ * 8 - 1)
                                                 ));
            }

            switch (facing) {
                case EnumFacing::NORTH:
                    this->offset(j * 8, l * 4, -(k + p_i45590_6_) * 8 + 1);
                    break;

                case EnumFacing::SOUTH:
                    this->offset(j * 8, l * 4, k * 8);
                    break;

                case EnumFacing::WEST:
                    this->offset(-(k + p_i45590_6_) * 8 + 1, l * 4, j * 8);
                    break;

                default:
                    this->offset(k * 8, l * 4, j * 8);
            }
        }

        MU void writeStructureToNBT(NBTCompound& tagCompound) {}

        MU void readStructureFromNBT(NBTCompound& tagCompound, TemplateManager& p_143011_2_) {}

        void generateWaterBox(World& worldIn, BoundingBox sBBIn, int p_181655_3_, int p_181655_4_,
                              int p_181655_5_, int p_181655_6_, int p_181655_7_, int p_181655_8_, bool p_181655_9_) {
            for (int i = p_181655_4_; i <= p_181655_7_; ++i) {
                for (int j = p_181655_3_; j <= p_181655_6_; ++j) {
                    for (int k = p_181655_5_; k <= p_181655_8_; ++k) {
                        if (!p_181655_9_ ||
                            lce::blocks::isReplaceableBlock(this->getBlockStateFromPos(worldIn, j, i, k, sBBIn).m_id)) {
                            // this->getBlockStateFromPos(p_181655_1_, j, i, k, sBBIn).getMaterial() != Material::AIR) {
                            if (this->getWorldY(i) >= World::getSeaLevel()) {
                                this->setBlockState(worldIn, sBBIn, j, i, k, lce::BlocksInit::AIR.getDefaultState());
                            } else {
                                this->setBlockState(worldIn, sBBIn, j, i, k, WATER);
                            }
                        }
                    }
                }
            }
        }

        void generateDefaultFloor(World& worldIn, BoundingBox sBBIn, int p_175821_3_, int p_175821_4_, bool p_175821_5_) {
            if (p_175821_5_) {
                this->fillWithBlocks(worldIn, sBBIn, p_175821_3_ + 0, 0, p_175821_4_ + 0, p_175821_3_ + 2, 0, p_175821_4_ + 8 - 1, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, p_175821_3_ + 5, 0, p_175821_4_ + 0, p_175821_3_ + 8 - 1, 0, p_175821_4_ + 8 - 1, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, p_175821_3_ + 3, 0, p_175821_4_ + 0, p_175821_3_ + 4, 0, p_175821_4_ + 2, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, p_175821_3_ + 3, 0, p_175821_4_ + 5, p_175821_3_ + 4, 0, p_175821_4_ + 8 - 1, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, p_175821_3_ + 3, 0, p_175821_4_ + 2, p_175821_3_ + 4, 0, p_175821_4_ + 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, p_175821_3_ + 3, 0, p_175821_4_ + 5, p_175821_3_ + 4, 0, p_175821_4_ + 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, p_175821_3_ + 2, 0, p_175821_4_ + 3, p_175821_3_ + 2, 0, p_175821_4_ + 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, p_175821_3_ + 5, 0, p_175821_4_ + 3, p_175821_3_ + 5, 0, p_175821_4_ + 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            } else {
                this->fillWithBlocks(worldIn, sBBIn, p_175821_3_ + 0, 0, p_175821_4_ + 0, p_175821_3_ + 8 - 1, 0, p_175821_4_ + 8 - 1, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            }
        }

        void generateBoxOnFillOnly(World& worldIn, BoundingBox sBBIn, int p_175819_3_, int p_175819_4_,
                                   int p_175819_5_, int p_175819_6_, int p_175819_7_, int p_175819_8_,
                                   lce::BlockState p_175819_9_) {
            for (int i = p_175819_4_; i <= p_175819_7_; ++i) {
                for (int j = p_175819_3_; j <= p_175819_6_; ++j) {
                    for (int k = p_175819_5_; k <= p_175819_8_; ++k) {
                        if (this->getBlockStateFromPos(worldIn, j, i, k, sBBIn) == WATER) {
                            this->setBlockState(worldIn, sBBIn, j, i, k, p_175819_9_);
                        }
                    }
                }
            }
        }

        bool doesChunkIntersect(BoundingBox sBBIn, int p_175818_2_, int p_175818_3_, int p_175818_4_, int p_175818_5_) {
            int i = this->getWorldX(p_175818_2_, p_175818_3_);
            int j = this->getWorldZ(p_175818_2_, p_175818_3_);
            int k = this->getWorldX(p_175818_4_, p_175818_5_);
            int l = this->getWorldZ(p_175818_4_, p_175818_5_);
            return sBBIn.intersects(BoundingBox(
                    static_cast<bbType_t>(std::min(i, k)),
                    static_cast<bbType_t>(std::min(j, l)),
                    static_cast<bbType_t>(std::max(i, k)),
                    static_cast<bbType_t>(std::max(j, l))
                    ));
        }

        /*
        bool spawnElder(World& worldIn, BoundingBox p_175817_2_, int p_175817_3_, int p_175817_4_, int p_175817_5_) {
            int i = this->getWorldX(p_175817_3_, p_175817_5_);
            int j = this->getWorldY(p_175817_4_);
            int k = this->getWorldZ(p_175817_3_, p_175817_5_);
            if (p_175817_2_.isVecInside(Pos3D(i, j, k))) {
                EntityElderGuardian entityelderguardian = new EntityElderGuardian(worldIn);
                entityelderguardian.heal(entityelderguardian.getMaxHealth());
                entityelderguardian.setLocationAndAngles((double) i + 0.5, (double) j, (double) k + 0.5, 0.0F, 0.0F);
                entityelderguardian.onInitialSpawn(worldIn.getDifficultyForLocation(new BlockPos(entityelderguardian)),
                                                   (IEntityLivingData) nullptr);
                worldIn.spawnEntityInWorld(entityelderguardian);
                return true;
            } else {
                return false;
            }
        }*/
    };
    /*
    static void registerOceanMonumentPieces() {
        MapGenStructureIO.registerStructureComponent(StructureOceanMonumentPieces::MonumentBuilding.class, "OMB");
        MapGenStructureIO.registerStructureComponent(StructureOceanMonumentPieces::MonumentCoreRoom.class, "OMCR");
        MapGenStructureIO.registerStructureComponent(StructureOceanMonumentPieces::DoubleXRoom.class, "OMDXR");
        MapGenStructureIO.registerStructureComponent(StructureOceanMonumentPieces::DoubleXYRoom.class, "OMDXYR");
        MapGenStructureIO.registerStructureComponent(StructureOceanMonumentPieces::DoubleYRoom.class, "OMDYR");
        MapGenStructureIO.registerStructureComponent(StructureOceanMonumentPieces::DoubleYZRoom.class, "OMDYZR");
        MapGenStructureIO.registerStructureComponent(StructureOceanMonumentPieces::DoubleZRoom.class, "OMDZR");
        MapGenStructureIO.registerStructureComponent(StructureOceanMonumentPieces::EntryRoom.class, "OMEntry");
        MapGenStructureIO.registerStructureComponent(StructureOceanMonumentPieces::Penthouse.class, "OMPenthouse");
        MapGenStructureIO.registerStructureComponent(StructureOceanMonumentPieces::SimpleRoom.class, "OMSimple");
        MapGenStructureIO.registerStructureComponent(StructureOceanMonumentPieces::SimpleTopRoom.class, "OMSimpleT");
    }
     */

    // interface
    class MonumentRoomFitHelper {
    public:
        virtual ~MonumentRoomFitHelper() = default;

        virtual bool fits(const RoomDefinition& definition) { return false; };

        virtual OceanMonumentPiece create(EnumFacing facing, RoomDefinition& definition, RNG& rng) {
            return OceanMonumentPiece();
        };
    };

    // implements

    class MU MonumentBuilding : public OceanMonumentPiece {
        std::vector<RoomDefinition> rooms;

        RoomDefinition* sourceRoom = nullptr;
        RoomDefinition* coreRoom   = nullptr;

        std::list<OceanMonumentPiece> childPieces = {};

    public:
        MonumentBuilding() = default;

        MU MonumentBuilding(RNG& rng, int p_i45599_2_, int p_i45599_3_, EnumFacing facing) : OceanMonumentPiece(0) {
            this->setCoordMode(facing);
            EnumFacing enumfacing = this->m_facing; // was getCoordBaseMode

            // holy stinky java code
            // if (getAxis(enumfacing) == EnumAxis::Z) {
            this->setBoundingBox(BoundingBox(
                    static_cast<bbType_t>(p_i45599_2_),
                    static_cast<bbType_t>(39),
                    static_cast<bbType_t>(p_i45599_3_),
                    static_cast<bbType_t>(p_i45599_2_ + 58 - 1),
                    static_cast<bbType_t>(61),
                    static_cast<bbType_t>(p_i45599_3_ + 58 - 1)
                    ));
            // } else {
            //     this->setBoundingBox(BoundingBox(
            //             static_cast<bbType_t>(p_i45599_2_),
            //             static_cast<bbType_t>(39),
            //             static_cast<bbType_t>(p_i45599_3_),
            //             static_cast<bbType_t>(p_i45599_2_ + 58 - 1),
            //             static_cast<bbType_t>(61),
            //             static_cast<bbType_t>(p_i45599_3_ + 58 - 1)
            //             ));
            // }

            std::vector<RoomDefinition*> list = this->generateRoomGraph(rng);
            if (this->sourceRoom != nullptr) {
                this->sourceRoom->claimed = true;
            }
            this->childPieces.push_back(EntryRoom(enumfacing, *this->sourceRoom));
            this->childPieces.push_back(MonumentCoreRoom(enumfacing, *this->coreRoom, rng));

            std::vector<std::unique_ptr<MonumentRoomFitHelper>> helpers;
            helpers.emplace_back(std::make_unique<XYDoubleRoomFitHelper>());
            helpers.emplace_back(std::make_unique<YZDoubleRoomFitHelper>());
            helpers.emplace_back(std::make_unique<ZDoubleRoomFitHelper>());
            helpers.emplace_back(std::make_unique<XDoubleRoomFitHelper>());
            helpers.emplace_back(std::make_unique<YDoubleRoomFitHelper>());
            helpers.emplace_back(std::make_unique<FitSimpleRoomTopHelper>());
            helpers.emplace_back(std::make_unique<FitSimpleRoomHelper>());

            // TODO: might be completely wrong
            for (RoomDefinition* room : list) {
                if (room->claimed || room->isSpecial()) continue;

                MonumentRoomFitHelper* chosen = nullptr;
                for (auto &h : helpers) {
                    if (h->fits(*room)) {
                        chosen = h.get();
                        break;
                    }
                }
                if (!chosen) continue;

                this->childPieces.push_back(chosen->create(enumfacing, *room, rng));
            }

            int j = this->m_minY;
            int k = this->getWorldX(9, 22);
            int l = this->getWorldZ(9, 22);

            for (OceanMonumentPiece& sop$piece: this->childPieces) {
                sop$piece.offset(k, j, l);
            }

            BoundingBox structureboundingbox1 = BoundingBox::createProper(
                    this->getWorldX(1, 1), this->getWorldY(1), this->getWorldZ(1, 1),
                    this->getWorldX(23, 21), this->getWorldY(8), this->getWorldZ(23, 21));
            BoundingBox structureboundingbox2 = BoundingBox::createProper(
                    this->getWorldX(34, 1), this->getWorldY(1), this->getWorldZ(34, 1),
                    this->getWorldX(56, 21), this->getWorldY(8), this->getWorldZ(56, 21));
            BoundingBox structureboundingbox = BoundingBox::createProper(
                    this->getWorldX(22, 22), this->getWorldY(13), this->getWorldZ(22, 22),
                    this->getWorldX(35, 35), this->getWorldY(17), this->getWorldZ(35, 35));
            int i = rng.nextInt();
            this->childPieces.push_back(WingRoom(enumfacing, structureboundingbox1, i++));
            this->childPieces.push_back(WingRoom(enumfacing, structureboundingbox2, i++));
            this->childPieces.push_back(Penthouse(enumfacing, structureboundingbox));
        }

        bool addComponentParts(World& worldIn, RNG& rng, BoundingBox sBBIn) {
            int i = std::max(World::getSeaLevel(), 64) - this->m_minY;
            this->generateWaterBox(worldIn, sBBIn, 0, 0, 0, 58, i, 58, false);
            this->generateWing(false, 0, worldIn, rng, sBBIn);
            this->generateWing(true, 33, worldIn, rng, sBBIn);
            this->generateEntranceArchs(worldIn, rng, sBBIn);
            this->generateEntranceWall(worldIn, rng, sBBIn);
            this->generateRoofPiece(worldIn, rng, sBBIn);
            this->generateLowerWall(worldIn, rng, sBBIn);
            this->generateMiddleWall(worldIn, rng, sBBIn);
            this->generateUpperWall(worldIn, rng, sBBIn);

            for (int j = 0; j < 7; ++j) {
                int k = 0;

                while (k < 7) {
                    if (k == 0 && j == 3) { k = 6; }

                    int l = j * 9;
                    int i1 = k * 9;

                    for (int j1 = 0; j1 < 4; ++j1) {
                        for (int k1 = 0; k1 < 4; ++k1) {
                            this->setBlockState(worldIn, sBBIn, l + j1, 0, i1 + k1, BRICKS_PRISMARINE);
                            this->replaceAirAndLiquidDownwards(worldIn, BRICKS_PRISMARINE, l + j1, -1, i1 + k1, sBBIn);
                        }
                    }

                    if (j != 0 && j != 6) {
                        k += 6;
                    } else {
                        ++k;
                    }
                }
            }

            for (int l1 = 0; l1 < 5; ++l1) {
                this->generateWaterBox(worldIn, sBBIn, -1 - l1, 0 + l1 * 2, -1 - l1, -1 - l1, 23, 58 + l1, false);
                this->generateWaterBox(worldIn, sBBIn, 58 + l1, 0 + l1 * 2, -1 - l1, 58 + l1, 23, 58 + l1, false);
                this->generateWaterBox(worldIn, sBBIn, 0 - l1, 0 + l1 * 2, -1 - l1, 57 + l1, 23, -1 - l1, false);
                this->generateWaterBox(worldIn, sBBIn, 0 - l1, 0 + l1 * 2, 58 + l1, 57 + l1, 23, 58 + l1, false);
            }

            for (OceanMonumentPiece& sop$piece: this->childPieces) {
                if (sop$piece.intersects(sBBIn)) {
                    switch (sop$piece.m_type) {
                        case PT_Monument_MonumentCoreRoom:
                            MonumentCoreRoom::addComponentParts(worldIn, rng, sBBIn, sop$piece);
                            break;
                        case PT_Monument_Penthouse:
                            Penthouse::addComponentParts(worldIn, rng, sBBIn, sop$piece);
                            break;
                        case PT_Monument_SimpleRoom:
                            SimpleRoom::addComponentParts(worldIn, rng, sBBIn, sop$piece);
                            break;
                        case PT_Monument_SimpleTopRoom:
                            SimpleTopRoom::addComponentParts(worldIn, rng, sBBIn, sop$piece);
                            break;
                        case PT_Monument_WingRoom:
                            WingRoom::addComponentParts(worldIn, rng, sBBIn, sop$piece);
                            break;
                        case PT_Monument_DoubleXRoom:
                            DoubleXRoom::addComponentParts(worldIn, rng, sBBIn, sop$piece);
                            break;
                        case PT_Monument_DoubleXYRoom:
                            DoubleXYRoom::addComponentParts(worldIn, rng, sBBIn, sop$piece);
                            break;
                        case PT_Monument_DoubleYRoom:
                            DoubleYRoom::addComponentParts(worldIn, rng, sBBIn, sop$piece);
                            break;
                        case PT_Monument_DoubleYZRoom:
                            DoubleYZRoom::addComponentParts(worldIn, rng, sBBIn, sop$piece);
                            break;
                        case PT_Monument_DoubleZRoom:
                            DoubleZRoom::addComponentParts(worldIn, rng, sBBIn, sop$piece);
                            break;
                        case PT_Monument_EntryRoom:
                            EntryRoom::addComponentParts(worldIn, rng, sBBIn, sop$piece);
                            break;
                        default:
                            break;
                    }
                }
            }

            return true;
        }

    private:
        std::vector<RoomDefinition*> generateRoomGraph(RNG& rng) {
            rooms.clear();
            rooms.reserve(75 + 3);
            rooms.resize(75);

            for (int i = 0; i < 5; ++i) {
                for (int j = 0; j < 4; ++j) {
                    int k = 0;
                    int l = getRoomIndex(i, 0, j);
                    rooms[l] = RoomDefinition(l);
                }
            }

            for (int i2 = 0; i2 < 5; ++i2) {
                for (int l2 = 0; l2 < 4; ++l2) {
                    int k3 = 1;
                    int j4 = getRoomIndex(i2, 1, l2);
                    rooms[j4] = RoomDefinition(j4);
                }
            }

            for (int j2 = 1; j2 < 4; ++j2) {
                for (int i3 = 0; i3 < 2; ++i3) {
                    int l3 = 2;
                    int k4 = getRoomIndex(j2, 2, i3);
                    rooms[k4] = RoomDefinition(k4);
                }
            }

            this->sourceRoom = &rooms[GRIDROOM_SOURCE_INDEX];

            for (int k2 = 0; k2 < 5; ++k2) {
                for (int j3 = 0; j3 < 5; ++j3) {
                    for (int i4 = 0; i4 < 3; ++i4) {
                        int l4 = getRoomIndex(k2, i4, j3);

                        if (rooms[l4].index != -1) {
                            for (const EnumFacing& enumfacing: FACING_VALUES) {
                                int i1 = k2 + getFrontOffsetX(enumfacing);
                                int j1 = i4 + getFrontOffsetY(enumfacing);
                                int k1 = j3 + getFrontOffsetZ(enumfacing);

                                if (i1 >= 0 && i1 < 5 && k1 >= 0 && k1 < 5 && j1 >= 0 && j1 < 3) {
                                    int l1 = getRoomIndex(i1, j1, k1);

                                    if (rooms[l1].index != -1) {
                                        if (k1 == j3) {
                                            rooms[l4].setConnection(enumfacing, rooms[l1]);
                                        } else {
                                            rooms[l4].setConnection(getOppositeFacing(enumfacing), rooms[l1]);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            rooms.emplace_back(1003);
            RoomDefinition* sop$rm  = &rooms.back(); // 1003

            rooms.emplace_back(1001);
            RoomDefinition* sop$rm1 = &rooms.back(); // 1001

            rooms.emplace_back(1002);
            RoomDefinition* sop$rm2 = &rooms.back(); // 1002

            // Connect special rooms to grid rooms.
            rooms[GRIDROOM_TOP_CONNECT_INDEX].setConnection(
                    EnumFacing::UP, *sop$rm);
            rooms[GRIDROOM_LEFTWING_CONNECT_INDEX].setConnection(
                    EnumFacing::SOUTH, *sop$rm1);
            rooms[GRIDROOM_RIGHTWING_CONNECT_INDEX].setConnection(
                    EnumFacing::SOUTH, *sop$rm2);

            sop$rm->claimed = true;
            sop$rm1->claimed = true;
            sop$rm2->claimed = true;

            this->sourceRoom->isSource = true;

            this->coreRoom = &rooms[getRoomIndex(rng.nextInt(4), 0, 2)];
            this->coreRoom->claimed = true;
            this->coreRoom->connections[getIndexFacing(EnumFacing::EAST)]->claimed = true;
            this->coreRoom->connections[getIndexFacing(EnumFacing::NORTH)]->claimed = true;
            this->coreRoom->connections[getIndexFacing(EnumFacing::EAST)]
                    ->connections[getIndexFacing(EnumFacing::NORTH)]
                    ->claimed = true;
            this->coreRoom->connections[getIndexFacing(EnumFacing::UP)]->claimed = true;
            this->coreRoom->connections[getIndexFacing(EnumFacing::EAST)]
                    ->connections[getIndexFacing(EnumFacing::UP)]
                    ->claimed = true;
            this->coreRoom->connections[getIndexFacing(EnumFacing::NORTH)]
                    ->connections[getIndexFacing(EnumFacing::UP)]
                    ->claimed = true;
            this->coreRoom->connections[getIndexFacing(EnumFacing::EAST)]
                    ->connections[getIndexFacing(EnumFacing::NORTH)]
                    ->connections[getIndexFacing(EnumFacing::UP)]
                    ->claimed = true;

            std::vector<RoomDefinition*> list;
            list.reserve(rooms.size());

            for (std::size_t idx = 0; idx < 75; ++idx) {
                RoomDefinition& room = rooms[idx];
                if (room.index != -1) {
                    room.updateOpenings();
                    list.push_back(&room);
                }
            }

            sop$rm->updateOpenings();
            Collections_shuffle(list, rng);
            int i5 = 1;

            for (RoomDefinition* sop$rm3: list) {
                int j5 = 0;
                int k5 = 0;

                while (j5 < 2 && k5 < 5) {
                    ++k5;
                    int l5 = rng.nextInt(6);

                    if (sop$rm3->hasOpening[l5]) {
                        int i6 = getIndexFacing(getOppositeFacing(getFront(l5)));
                        sop$rm3->hasOpening[l5] = false;
                        sop$rm3->connections[l5]->hasOpening[i6] = false;

                        if (sop$rm3->findSource(i5++) &&
                            sop$rm3->connections[l5]->findSource(i5++)) {
                            ++j5;
                        } else {
                            sop$rm3->hasOpening[l5] = true;
                            sop$rm3->connections[l5]->hasOpening[i6] = true;
                        }
                    }
                }
            }

            list.push_back(sop$rm);
            list.push_back(sop$rm1);
            list.push_back(sop$rm2);
            return list;
        }

        void generateWing(bool p_175840_1_, int p_175840_2_, World& worldIn, RNG& rng, BoundingBox sBBIn) {
            int i = 24;

            if (this->doesChunkIntersect(sBBIn, p_175840_2_, 0, p_175840_2_ + 23, 20)) {
                this->fillWithBlocks(worldIn, sBBIn, p_175840_2_ + 0, 0, 0, p_175840_2_ + 24, 0, 20, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->generateWaterBox(worldIn, sBBIn, p_175840_2_ + 0, 1, 0, p_175840_2_ + 24, 10, 20, false);

                for (int j = 0; j < 4; ++j) {
                    this->fillWithBlocks(worldIn, sBBIn, p_175840_2_ + j, j + 1, j, p_175840_2_ + j, j + 1, 20, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    this->fillWithBlocks(worldIn, sBBIn, p_175840_2_ + j + 7, j + 5, j + 7, p_175840_2_ + j + 7, j + 5, 20, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    this->fillWithBlocks(worldIn, sBBIn, p_175840_2_ + 17 - j, j + 5, j + 7, p_175840_2_ + 17 - j, j + 5, 20, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    this->fillWithBlocks(worldIn, sBBIn, p_175840_2_ + 24 - j, j + 1, j, p_175840_2_ + 24 - j, j + 1, 20, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    this->fillWithBlocks(worldIn, sBBIn, p_175840_2_ + j + 1, j + 1, j, p_175840_2_ + 23 - j, j + 1, j, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    this->fillWithBlocks(worldIn, sBBIn, p_175840_2_ + j + 8, j + 5, j + 7, p_175840_2_ + 16 - j, j + 5, j + 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                }

                this->fillWithBlocks(worldIn, sBBIn, p_175840_2_ + 4, 4, 4, p_175840_2_ + 6, 4, 20, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, p_175840_2_ + 7, 4, 4, p_175840_2_ + 17, 4, 6, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, p_175840_2_ + 18, 4, 4, p_175840_2_ + 20, 4, 20, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, p_175840_2_ + 11, 8, 11, p_175840_2_ + 13, 8, 20, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->setBlockState(worldIn, sBBIn, p_175840_2_ + 12, 9, 12, DOT_DECO_DATA);
                this->setBlockState(worldIn, sBBIn, p_175840_2_ + 12, 9, 15, DOT_DECO_DATA);
                this->setBlockState(worldIn, sBBIn, p_175840_2_ + 12, 9, 18, DOT_DECO_DATA);
                int j1 = p_175840_2_ + (p_175840_1_ ? 19 : 5);
                int k = p_175840_2_ + (p_175840_1_ ? 5 : 19);

                for (int l = 20; l >= 5; l -= 3) { this->setBlockState(worldIn, sBBIn, j1, 5, l, DOT_DECO_DATA); }

                for (int k1 = 19; k1 >= 7; k1 -= 3) { this->setBlockState(worldIn, sBBIn, k, 5, k1, DOT_DECO_DATA); }

                for (int l1 = 0; l1 < 4; ++l1) {
                    int i1 = p_175840_1_ ? p_175840_2_ + (24 - (17 - l1 * 3)) : p_175840_2_ + 17 - l1 * 3;
                    this->setBlockState(worldIn, sBBIn, i1, 5, 5, DOT_DECO_DATA);
                }

                this->setBlockState(worldIn, sBBIn, k, 5, 5, DOT_DECO_DATA);
                this->fillWithBlocks(worldIn, sBBIn, p_175840_2_ + 11, 1, 12, p_175840_2_ + 13, 7, 12, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, p_175840_2_ + 12, 1, 11, p_175840_2_ + 12, 7, 13, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            }
        }

        void generateEntranceArchs(World& worldIn, RNG& rng, BoundingBox sBBIn) {
            if (this->doesChunkIntersect(sBBIn, 22, 5, 35, 17)) {
                this->generateWaterBox(worldIn, sBBIn, 25, 0, 0, 32, 8, 20, false);

                for (int i = 0; i < 4; ++i) {
                    this->fillWithBlocks(worldIn, sBBIn, 24, 2, 5 + i * 4, 24, 4, 5 + i * 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    this->fillWithBlocks(worldIn, sBBIn, 22, 4, 5 + i * 4, 23, 4, 5 + i * 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    this->setBlockState(worldIn, sBBIn, 25, 5, 5 + i * 4, BRICKS_PRISMARINE);
                    this->setBlockState(worldIn, sBBIn, 26, 6, 5 + i * 4, BRICKS_PRISMARINE);
                    this->setBlockState(worldIn, sBBIn, 26, 5, 5 + i * 4, SEA_LANTERN);
                    this->fillWithBlocks(worldIn, sBBIn, 33, 2, 5 + i * 4, 33, 4, 5 + i * 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    this->fillWithBlocks(worldIn, sBBIn, 34, 4, 5 + i * 4, 35, 4, 5 + i * 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    this->setBlockState(worldIn, sBBIn, 32, 5, 5 + i * 4, BRICKS_PRISMARINE);
                    this->setBlockState(worldIn, sBBIn, 31, 6, 5 + i * 4, BRICKS_PRISMARINE);
                    this->setBlockState(worldIn, sBBIn, 31, 5, 5 + i * 4, SEA_LANTERN);
                    this->fillWithBlocks(worldIn, sBBIn, 27, 6, 5 + i * 4, 30, 6, 5 + i * 4, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                }
            }
        }

        void generateEntranceWall(World& worldIn, RNG& rng, BoundingBox sBBIn) {
            if (this->doesChunkIntersect(sBBIn, 15, 20, 42, 21)) {
                this->fillWithBlocks(worldIn, sBBIn, 15, 0, 21, 42, 0, 21, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->generateWaterBox(worldIn, sBBIn, 26, 1, 21, 31, 3, 21, false);
                this->fillWithBlocks(worldIn, sBBIn, 21, 12, 21, 36, 12, 21, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 17, 11, 21, 40, 11, 21, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 16, 10, 21, 41, 10, 21, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 15, 7, 21, 42, 9, 21, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 16, 6, 21, 41, 6, 21, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 17, 5, 21, 40, 5, 21, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 21, 4, 21, 36, 4, 21, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 22, 3, 21, 26, 3, 21, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 31, 3, 21, 35, 3, 21, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 23, 2, 21, 25, 2, 21, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 32, 2, 21, 34, 2, 21, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 28, 4, 20, 29, 4, 21, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                this->setBlockState(worldIn, sBBIn, 27, 3, 21, BRICKS_PRISMARINE);
                this->setBlockState(worldIn, sBBIn, 30, 3, 21, BRICKS_PRISMARINE);
                this->setBlockState(worldIn, sBBIn, 26, 2, 21, BRICKS_PRISMARINE);
                this->setBlockState(worldIn, sBBIn, 31, 2, 21, BRICKS_PRISMARINE);
                this->setBlockState(worldIn, sBBIn, 25, 1, 21, BRICKS_PRISMARINE);
                this->setBlockState(worldIn, sBBIn, 32, 1, 21, BRICKS_PRISMARINE);

                for (int i = 0; i < 7; ++i) {
                    this->setBlockState(worldIn, sBBIn, 28 - i, 6 + i, 21, DARK_PRISMARINE);
                    this->setBlockState(worldIn, sBBIn, 29 + i, 6 + i, 21, DARK_PRISMARINE);
                }

                for (int j = 0; j < 4; ++j) {
                    this->setBlockState(worldIn, sBBIn, 28 - j, 9 + j, 21, DARK_PRISMARINE);
                    this->setBlockState(worldIn, sBBIn, 29 + j, 9 + j, 21, DARK_PRISMARINE);
                }

                this->setBlockState(worldIn, sBBIn, 28, 12, 21, DARK_PRISMARINE);
                this->setBlockState(worldIn, sBBIn, 29, 12, 21, DARK_PRISMARINE);

                for (int k = 0; k < 3; ++k) {
                    this->setBlockState(worldIn, sBBIn, 22 - k * 2, 8, 21, DARK_PRISMARINE);
                    this->setBlockState(worldIn, sBBIn, 22 - k * 2, 9, 21, DARK_PRISMARINE);
                    this->setBlockState(worldIn, sBBIn, 35 + k * 2, 8, 21, DARK_PRISMARINE);
                    this->setBlockState(worldIn, sBBIn, 35 + k * 2, 9, 21, DARK_PRISMARINE);
                }

                this->generateWaterBox(worldIn, sBBIn, 15, 13, 21, 42, 15, 21, false);
                this->generateWaterBox(worldIn, sBBIn, 15, 1, 21, 15, 6, 21, false);
                this->generateWaterBox(worldIn, sBBIn, 16, 1, 21, 16, 5, 21, false);
                this->generateWaterBox(worldIn, sBBIn, 17, 1, 21, 20, 4, 21, false);
                this->generateWaterBox(worldIn, sBBIn, 21, 1, 21, 21, 3, 21, false);
                this->generateWaterBox(worldIn, sBBIn, 22, 1, 21, 22, 2, 21, false);
                this->generateWaterBox(worldIn, sBBIn, 23, 1, 21, 24, 1, 21, false);
                this->generateWaterBox(worldIn, sBBIn, 42, 1, 21, 42, 6, 21, false);
                this->generateWaterBox(worldIn, sBBIn, 41, 1, 21, 41, 5, 21, false);
                this->generateWaterBox(worldIn, sBBIn, 37, 1, 21, 40, 4, 21, false);
                this->generateWaterBox(worldIn, sBBIn, 36, 1, 21, 36, 3, 21, false);
                this->generateWaterBox(worldIn, sBBIn, 33, 1, 21, 34, 1, 21, false);
                this->generateWaterBox(worldIn, sBBIn, 35, 1, 21, 35, 2, 21, false);
            }
        }

        void generateRoofPiece(World& worldIn, RNG& rng, BoundingBox sBBIn) {
            if (this->doesChunkIntersect(sBBIn, 21, 21, 36, 36)) {
                this->fillWithBlocks(worldIn, sBBIn, 21, 0, 22, 36, 0, 36, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->generateWaterBox(worldIn, sBBIn, 21, 1, 22, 36, 23, 36, false);

                for (int i = 0; i < 4; ++i) {
                    this->fillWithBlocks(worldIn, sBBIn, 21 + i, 13 + i, 21 + i, 36 - i, 13 + i, 21 + i, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    this->fillWithBlocks(worldIn, sBBIn, 21 + i, 13 + i, 36 - i, 36 - i, 13 + i, 36 - i, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    this->fillWithBlocks(worldIn, sBBIn, 21 + i, 13 + i, 22 + i, 21 + i, 13 + i, 35 - i, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    this->fillWithBlocks(worldIn, sBBIn, 36 - i, 13 + i, 22 + i, 36 - i, 13 + i, 35 - i, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                }

                this->fillWithBlocks(worldIn, sBBIn, 25, 16, 25, 32, 16, 32, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 25, 17, 25, 25, 19, 25, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 32, 17, 25, 32, 19, 25, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 25, 17, 32, 25, 19, 32, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 32, 17, 32, 32, 19, 32, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                this->setBlockState(worldIn, sBBIn, 26, 20, 26, BRICKS_PRISMARINE);
                this->setBlockState(worldIn, sBBIn, 27, 21, 27, BRICKS_PRISMARINE);
                this->setBlockState(worldIn, sBBIn, 27, 20, 27, SEA_LANTERN);
                this->setBlockState(worldIn, sBBIn, 26, 20, 31, BRICKS_PRISMARINE);
                this->setBlockState(worldIn, sBBIn, 27, 21, 30, BRICKS_PRISMARINE);
                this->setBlockState(worldIn, sBBIn, 27, 20, 30, SEA_LANTERN);
                this->setBlockState(worldIn, sBBIn, 31, 20, 31, BRICKS_PRISMARINE);
                this->setBlockState(worldIn, sBBIn, 30, 21, 30, BRICKS_PRISMARINE);
                this->setBlockState(worldIn, sBBIn, 30, 20, 30, SEA_LANTERN);
                this->setBlockState(worldIn, sBBIn, 31, 20, 26, BRICKS_PRISMARINE);
                this->setBlockState(worldIn, sBBIn, 30, 21, 27, BRICKS_PRISMARINE);
                this->setBlockState(worldIn, sBBIn, 30, 20, 27, SEA_LANTERN);
                this->fillWithBlocks(worldIn, sBBIn, 28, 21, 27, 29, 21, 27, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 27, 21, 28, 27, 21, 29, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 28, 21, 30, 29, 21, 30, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 30, 21, 28, 30, 21, 29, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            }
        }

        void generateLowerWall(World& worldIn, RNG& rng, BoundingBox sBBIn) {
            if (this->doesChunkIntersect(sBBIn, 0, 21, 6, 58)) {
                this->fillWithBlocks(worldIn, sBBIn, 0, 0, 21, 6, 0, 57, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->generateWaterBox(worldIn, sBBIn, 0, 1, 21, 6, 7, 57, false);
                this->fillWithBlocks(worldIn, sBBIn, 4, 4, 21, 6, 4, 53, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);

                for (int i = 0; i < 4; ++i) {
                    this->fillWithBlocks(worldIn, sBBIn, i, i + 1, 21, i, i + 1, 57 - i, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                }

                for (int j = 23; j < 53; j += 3) { this->setBlockState(worldIn, sBBIn, 5, 5, j, DOT_DECO_DATA); }

                this->setBlockState(worldIn, sBBIn, 5, 5, 52, DOT_DECO_DATA);

                for (int k = 0; k < 4; ++k) {
                    this->fillWithBlocks(worldIn, sBBIn, k, k + 1, 21, k, k + 1, 57 - k, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                }

                this->fillWithBlocks(worldIn, sBBIn, 4, 1, 52, 6, 3, 52, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 5, 1, 51, 5, 3, 53, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            }

            if (this->doesChunkIntersect(sBBIn, 51, 21, 58, 58)) {
                this->fillWithBlocks(worldIn, sBBIn, 51, 0, 21, 57, 0, 57, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->generateWaterBox(worldIn, sBBIn, 51, 1, 21, 57, 7, 57, false);
                this->fillWithBlocks(worldIn, sBBIn, 51, 4, 21, 53, 4, 53, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);

                for (int l = 0; l < 4; ++l) {
                    this->fillWithBlocks(worldIn, sBBIn, 57 - l, l + 1, 21, 57 - l, l + 1, 57 - l, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                }

                for (int i1 = 23; i1 < 53; i1 += 3) { this->setBlockState(worldIn, sBBIn, 52, 5, i1, DOT_DECO_DATA); }

                this->setBlockState(worldIn, sBBIn, 52, 5, 52, DOT_DECO_DATA);
                this->fillWithBlocks(worldIn, sBBIn, 51, 1, 52, 53, 3, 52, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 52, 1, 51, 52, 3, 53, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            }

            if (this->doesChunkIntersect(sBBIn, 0, 51, 57, 57)) {
                this->fillWithBlocks(worldIn, sBBIn, 7, 0, 51, 50, 0, 57, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->generateWaterBox(worldIn, sBBIn, 7, 1, 51, 50, 10, 57, false);

                for (int j1 = 0; j1 < 4; ++j1) {
                    this->fillWithBlocks(worldIn, sBBIn, j1 + 1, j1 + 1, 57 - j1, 56 - j1, j1 + 1, 57 - j1,
                                         BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                }
            }
        }

        void generateMiddleWall(World& worldIn, RNG& rng, BoundingBox sBBIn) {
            if (this->doesChunkIntersect(sBBIn, 7, 21, 13, 50)) {
                this->fillWithBlocks(worldIn, sBBIn, 7, 0, 21, 13, 0, 50, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->generateWaterBox(worldIn, sBBIn, 7, 1, 21, 13, 10, 50, false);
                this->fillWithBlocks(worldIn, sBBIn, 11, 8, 21, 13, 8, 53, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);

                for (int i = 0; i < 4; ++i) {
                    this->fillWithBlocks(worldIn, sBBIn, i + 7, i + 5, 21, i + 7, i + 5, 54, BRICKS_PRISMARINE,
                                         BRICKS_PRISMARINE, false);
                }

                for (int j = 21; j <= 45; j += 3) { this->setBlockState(worldIn, sBBIn, 12, 9, j, DOT_DECO_DATA); }
            }

            if (this->doesChunkIntersect(sBBIn, 44, 21, 50, 54)) {
                this->fillWithBlocks(worldIn, sBBIn, 44, 0, 21, 50, 0, 50, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->generateWaterBox(worldIn, sBBIn, 44, 1, 21, 50, 10, 50, false);
                this->fillWithBlocks(worldIn, sBBIn, 44, 8, 21, 46, 8, 53, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);

                for (int k = 0; k < 4; ++k) {
                    this->fillWithBlocks(worldIn, sBBIn, 50 - k, k + 5, 21, 50 - k, k + 5, 54, BRICKS_PRISMARINE,
                                         BRICKS_PRISMARINE, false);
                }

                for (int l = 21; l <= 45; l += 3) { this->setBlockState(worldIn, sBBIn, 45, 9, l, DOT_DECO_DATA); }
            }

            if (this->doesChunkIntersect(sBBIn, 8, 44, 49, 54)) {
                this->fillWithBlocks(worldIn, sBBIn, 14, 0, 44, 43, 0, 50, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->generateWaterBox(worldIn, sBBIn, 14, 1, 44, 43, 10, 50, false);

                for (int i1 = 12; i1 <= 45; i1 += 3) {
                    this->setBlockState(worldIn, sBBIn, i1, 9, 45, DOT_DECO_DATA);
                    this->setBlockState(worldIn, sBBIn, i1, 9, 52, DOT_DECO_DATA);

                    if (i1 == 12 || i1 == 18 || i1 == 24 || i1 == 33 || i1 == 39 || i1 == 45) {
                        this->setBlockState(worldIn, sBBIn, i1, 9, 47, DOT_DECO_DATA);
                        this->setBlockState(worldIn, sBBIn, i1, 9, 50, DOT_DECO_DATA);
                        this->setBlockState(worldIn, sBBIn, i1, 10, 45, DOT_DECO_DATA);
                        this->setBlockState(worldIn, sBBIn, i1, 10, 46, DOT_DECO_DATA);
                        this->setBlockState(worldIn, sBBIn, i1, 10, 51, DOT_DECO_DATA);
                        this->setBlockState(worldIn, sBBIn, i1, 10, 52, DOT_DECO_DATA);
                        this->setBlockState(worldIn, sBBIn, i1, 11, 47, DOT_DECO_DATA);
                        this->setBlockState(worldIn, sBBIn, i1, 11, 50, DOT_DECO_DATA);
                        this->setBlockState(worldIn, sBBIn, i1, 12, 48, DOT_DECO_DATA);
                        this->setBlockState(worldIn, sBBIn, i1, 12, 49, DOT_DECO_DATA);
                    }
                }

                for (int j1 = 0; j1 < 3; ++j1) {
                    this->fillWithBlocks(worldIn, sBBIn, 8 + j1, 5 + j1, 54, 49 - j1, 5 + j1, 54, ROUGH_PRISMARINE,
                                         ROUGH_PRISMARINE, false);
                }

                this->fillWithBlocks(worldIn, sBBIn, 11, 8, 54, 46, 8, 54, BRICKS_PRISMARINE, BRICKS_PRISMARINE,
                                     false);
                this->fillWithBlocks(worldIn, sBBIn, 14, 8, 44, 43, 8, 53, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            }
        }

        void generateUpperWall(World& worldIn, RNG& rng, BoundingBox sBBIn) {
            if (this->doesChunkIntersect(sBBIn, 14, 21, 20, 43)) {
                this->fillWithBlocks(worldIn, sBBIn, 14, 0, 21, 20, 0, 43, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->generateWaterBox(worldIn, sBBIn, 14, 1, 22, 20, 14, 43, false);
                this->fillWithBlocks(worldIn, sBBIn, 18, 12, 22, 20, 12, 39, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 18, 12, 21, 20, 12, 21, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);

                for (int i = 0; i < 4; ++i) {
                    this->fillWithBlocks(worldIn, sBBIn, i + 14, i + 9, 21, i + 14, i + 9, 43 - i, BRICKS_PRISMARINE,
                                         BRICKS_PRISMARINE, false);
                }

                for (int j = 23; j <= 39; j += 3) { this->setBlockState(worldIn, sBBIn, 19, 13, j, DOT_DECO_DATA); }
            }

            if (this->doesChunkIntersect(sBBIn, 37, 21, 43, 43)) {
                this->fillWithBlocks(worldIn, sBBIn, 37, 0, 21, 43, 0, 43, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->generateWaterBox(worldIn, sBBIn, 37, 1, 22, 43, 14, 43, false);
                this->fillWithBlocks(worldIn, sBBIn, 37, 12, 22, 39, 12, 39, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->fillWithBlocks(worldIn, sBBIn, 37, 12, 21, 39, 12, 21, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);

                for (int k = 0; k < 4; ++k) {
                    this->fillWithBlocks(worldIn, sBBIn, 43 - k, k + 9, 21, 43 - k, k + 9, 43 - k, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                }

                for (int l = 23; l <= 39; l += 3) { this->setBlockState(worldIn, sBBIn, 38, 13, l, DOT_DECO_DATA); }
            }

            if (this->doesChunkIntersect(sBBIn, 15, 37, 42, 43)) {
                this->fillWithBlocks(worldIn, sBBIn, 21, 0, 37, 36, 0, 43, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                this->generateWaterBox(worldIn, sBBIn, 21, 1, 37, 36, 14, 43, false);
                this->fillWithBlocks(worldIn, sBBIn, 21, 12, 37, 36, 12, 39, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);

                for (int i1 = 0; i1 < 4; ++i1) {
                    this->fillWithBlocks(worldIn, sBBIn, 15 + i1, i1 + 9, 43 - i1, 42 - i1, i1 + 9, 43 - i1, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                }

                for (int j1 = 21; j1 <= 36; j1 += 3) {
                    this->setBlockState(worldIn, sBBIn, j1, 13, 38, DOT_DECO_DATA);
                }
            }
        }
    };

    class MonumentCoreRoom : public OceanMonumentPiece {
public:
    MonumentCoreRoom() = delete;

    MonumentCoreRoom(EnumFacing facing, RoomDefinition& definition, RNG& rng)
        : OceanMonumentPiece(PT_Monument_MonumentCoreRoom, facing, &definition, 2, 2, 2) {
    }

    static bool addComponentParts(World& worldIn, RNG& rng, BoundingBox sBBIn, OceanMonumentPiece& piece)  {
        piece.generateBoxOnFillOnly(worldIn, sBBIn, 1, 8, 0, 14, 8, 14, ROUGH_PRISMARINE);
        int i = 7;
        lce::BlockState b = BRICKS_PRISMARINE;
        piece.fillWithBlocks(worldIn, sBBIn, 0, 7, 0, 0, 7, 15, b, b, false);
        piece.fillWithBlocks(worldIn, sBBIn, 15, 7, 0, 15, 7, 15, b, b, false);
        piece.fillWithBlocks(worldIn, sBBIn, 1, 7, 0, 15, 7, 0, b, b, false);
        piece.fillWithBlocks(worldIn, sBBIn, 1, 7, 15, 14, 7, 15, b, b, false);

        for (int k = 1; k <= 6; ++k) {
            b = BRICKS_PRISMARINE;

            if (k == 2 || k == 6) { b = ROUGH_PRISMARINE; }

            for (int j = 0; j <= 15; j += 15) {
                piece.fillWithBlocks(worldIn, sBBIn, j, k, 0, j, k, 1, b, b, false);
                piece.fillWithBlocks(worldIn, sBBIn, j, k, 6, j, k, 9, b, b, false);
                piece.fillWithBlocks(worldIn, sBBIn, j, k, 14, j, k, 15, b, b, false);
            }

            piece.fillWithBlocks(worldIn, sBBIn, 1, k, 0, 1, k, 0, b, b, false);
            piece.fillWithBlocks(worldIn, sBBIn, 6, k, 0, 9, k, 0, b, b, false);
            piece.fillWithBlocks(worldIn, sBBIn, 14, k, 0, 14, k, 0, b, b, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, k, 15, 14, k, 15, b, b, false);
        }

        piece.fillWithBlocks(worldIn, sBBIn, 6, 3, 6, 9, 6, 9, DARK_PRISMARINE, DARK_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 7, 4, 7, 8, 5, 8, lce::BlocksInit::GOLD_BLOCK.getDefaultState(),
                             lce::BlocksInit::GOLD_BLOCK.getDefaultState(), false);

        for (int l = 3; l <= 6; l += 3) {
            for (int i1 = 6; i1 <= 9; i1 += 3) {
                piece.setBlockState(worldIn, sBBIn, i1, l, 6, SEA_LANTERN);
                piece.setBlockState(worldIn, sBBIn, i1, l, 9, SEA_LANTERN);
            }
        }

        piece.fillWithBlocks(worldIn, sBBIn, 5, 1, 6, 5, 2, 6, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 5, 1, 9, 5, 2, 9, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 10, 1, 6, 10, 2, 6, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 10, 1, 9, 10, 2, 9, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 6, 1, 5, 6, 2, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 9, 1, 5, 9, 2, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 6, 1, 10, 6, 2, 10, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 9, 1, 10, 9, 2, 10, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 5, 2, 5, 5, 6, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 5, 2, 10, 5, 6, 10, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 10, 2, 5, 10, 6, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 10, 2, 10, 10, 6, 10, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 5, 7, 1, 5, 7, 6, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 10, 7, 1, 10, 7, 6, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 5, 7, 9, 5, 7, 14, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 10, 7, 9, 10, 7, 14, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 1, 7, 5, 6, 7, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 1, 7, 10, 6, 7, 10, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 9, 7, 5, 14, 7, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 9, 7, 10, 14, 7, 10, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 2, 1, 2, 2, 1, 3, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 3, 1, 2, 3, 1, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 13, 1, 2, 13, 1, 3, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 12, 1, 2, 12, 1, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 2, 1, 12, 2, 1, 13, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 3, 1, 13, 3, 1, 13, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 13, 1, 12, 13, 1, 13, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 12, 1, 13, 12, 1, 13, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        return true;
    }
};

    class Penthouse : public OceanMonumentPiece {
public:
    Penthouse() = delete;

    Penthouse(EnumFacing facing, BoundingBox definition) : OceanMonumentPiece(facing, definition) {
        this->m_type = PT_Monument_Penthouse;
    }

    static bool addComponentParts(World& worldIn, RNG& rng, BoundingBox sBBIn, OceanMonumentPiece& piece)  {
        piece.fillWithBlocks(worldIn, sBBIn, 2, -1, 2, 11, -1, 11, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 0, -1, 0, 1, -1, 11, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 12, -1, 0, 13, -1, 11, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 2, -1, 0, 11, -1, 1, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 2, -1, 12, 11, -1, 13, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 0, 0, 0, 0, 0, 13, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 13, 0, 0, 13, 0, 13, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 1, 0, 0, 12, 0, 0, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 1, 0, 13, 12, 0, 13, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);

        for (int i = 2; i <= 11; i += 3) {
            piece.setBlockState(worldIn, sBBIn, 0, 0, i, SEA_LANTERN);
            piece.setBlockState(worldIn, sBBIn, 13, 0, i, SEA_LANTERN);
            piece.setBlockState(worldIn, sBBIn, i, 0, 0, SEA_LANTERN);
        }

        piece.fillWithBlocks(worldIn, sBBIn, 2, 0, 3, 4, 0, 9, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 9, 0, 3, 11, 0, 9, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 4, 0, 9, 9, 0, 11, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.setBlockState(worldIn, sBBIn, 5, 0, 8, BRICKS_PRISMARINE);
        piece.setBlockState(worldIn, sBBIn, 8, 0, 8, BRICKS_PRISMARINE);
        piece.setBlockState(worldIn, sBBIn, 10, 0, 10, BRICKS_PRISMARINE);
        piece.setBlockState(worldIn, sBBIn, 3, 0, 10, BRICKS_PRISMARINE);
        piece.fillWithBlocks(worldIn, sBBIn, 3, 0, 3, 3, 0, 7, DARK_PRISMARINE, DARK_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 10, 0, 3, 10, 0, 7, DARK_PRISMARINE, DARK_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 6, 0, 10, 7, 0, 10, DARK_PRISMARINE, DARK_PRISMARINE, false);
        int l = 3;

        for (int j = 0; j < 2; ++j) {
            for (int k = 2; k <= 8; k += 3) {
                piece.fillWithBlocks(worldIn, sBBIn, l, 0, k, l, 2, k, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            }

            l = 10;
        }

        piece.fillWithBlocks(worldIn, sBBIn, 5, 0, 10, 5, 2, 10, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 8, 0, 10, 8, 2, 10, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 6, -1, 7, 7, -1, 8, DARK_PRISMARINE, DARK_PRISMARINE, false);
        piece.generateWaterBox(worldIn, sBBIn, 6, -1, 3, 7, -1, 4, false);
        // piece.spawnElder(worldIn, sBBIn, 6, 1, 6);
        return true;
    }
};

    class SimpleRoom : public OceanMonumentPiece {
public:
    SimpleRoom() = delete;

    SimpleRoom(EnumFacing facing, RoomDefinition& definition, RNG& rng)
        : OceanMonumentPiece(PT_Monument_SimpleRoom, facing, &definition, 1, 1, 1) {
        this->m_data = rng.nextInt(3);
    }

    static bool addComponentParts(World& worldIn, RNG& rng, BoundingBox sBBIn, OceanMonumentPiece& piece)  {
        if (piece.roomDefinition->index / 25 > 0) {
            piece.generateDefaultFloor(worldIn, sBBIn, 0, 0,
                                       piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::DOWN)]);
        }

        if (piece.roomDefinition->connections[getIndexFacing(EnumFacing::UP)] == nullptr) {
            piece.generateBoxOnFillOnly(worldIn, sBBIn, 1, 4, 1, 6, 4, 6, ROUGH_PRISMARINE);
        }

        bool flag = piece.m_data != 0 && rng.nextBoolean() &&
                    !piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::DOWN)] &&
                    !piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::UP)] &&
                    piece.roomDefinition->countOpenings() > 1;

        if (piece.m_data == 0) {
            piece.fillWithBlocks(worldIn, sBBIn, 0, 1, 0, 2, 1, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 0, 3, 0, 2, 3, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 0, 2, 0, 0, 2, 2, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 2, 0, 2, 2, 0, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            piece.setBlockState(worldIn, sBBIn, 1, 2, 1, SEA_LANTERN);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 1, 0, 7, 1, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 3, 0, 7, 3, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 7, 2, 0, 7, 2, 2, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 2, 0, 6, 2, 0, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            piece.setBlockState(worldIn, sBBIn, 6, 2, 1, SEA_LANTERN);
            piece.fillWithBlocks(worldIn, sBBIn, 0, 1, 5, 2, 1, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 0, 3, 5, 2, 3, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 0, 2, 5, 0, 2, 7, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 2, 7, 2, 2, 7, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            piece.setBlockState(worldIn, sBBIn, 1, 2, 6, SEA_LANTERN);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 1, 5, 7, 1, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 3, 5, 7, 3, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 7, 2, 5, 7, 2, 7, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 2, 7, 6, 2, 7, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            piece.setBlockState(worldIn, sBBIn, 6, 2, 6, SEA_LANTERN);

            if (piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::SOUTH)]) {
                piece.fillWithBlocks(worldIn, sBBIn, 3, 3, 0, 4, 3, 0, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            } else {
                piece.fillWithBlocks(worldIn, sBBIn, 3, 3, 0, 4, 3, 1, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 3, 2, 0, 4, 2, 0, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 3, 1, 0, 4, 1, 1, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            }

            if (piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::NORTH)]) {
                piece.fillWithBlocks(worldIn, sBBIn, 3, 3, 7, 4, 3, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            } else {
                piece.fillWithBlocks(worldIn, sBBIn, 3, 3, 6, 4, 3, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 3, 2, 7, 4, 2, 7, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 3, 1, 6, 4, 1, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            }

            if (piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::WEST)]) {
                piece.fillWithBlocks(worldIn, sBBIn, 0, 3, 3, 0, 3, 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            } else {
                piece.fillWithBlocks(worldIn, sBBIn, 0, 3, 3, 1, 3, 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 0, 2, 3, 0, 2, 4, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 0, 1, 3, 1, 1, 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            }

            if (piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::EAST)]) {
                piece.fillWithBlocks(worldIn, sBBIn, 7, 3, 3, 7, 3, 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            } else {
                piece.fillWithBlocks(worldIn, sBBIn, 6, 3, 3, 7, 3, 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 7, 2, 3, 7, 2, 4, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 6, 1, 3, 7, 1, 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            }
        } else if (piece.m_data == 1) {
            piece.fillWithBlocks(worldIn, sBBIn, 2, 1, 2, 2, 3, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 2, 1, 5, 2, 3, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 1, 5, 5, 3, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 1, 2, 5, 3, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.setBlockState(worldIn, sBBIn, 2, 2, 2, SEA_LANTERN);
            piece.setBlockState(worldIn, sBBIn, 2, 2, 5, SEA_LANTERN);
            piece.setBlockState(worldIn, sBBIn, 5, 2, 5, SEA_LANTERN);
            piece.setBlockState(worldIn, sBBIn, 5, 2, 2, SEA_LANTERN);
            piece.fillWithBlocks(worldIn, sBBIn, 0, 1, 0, 1, 3, 0, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 0, 1, 1, 0, 3, 1, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 0, 1, 7, 1, 3, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 0, 1, 6, 0, 3, 6, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 6, 1, 7, 7, 3, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 7, 1, 6, 7, 3, 6, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 6, 1, 0, 7, 3, 0, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 7, 1, 1, 7, 3, 1, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.setBlockState(worldIn, sBBIn, 1, 2, 0, ROUGH_PRISMARINE);
            piece.setBlockState(worldIn, sBBIn, 0, 2, 1, ROUGH_PRISMARINE);
            piece.setBlockState(worldIn, sBBIn, 1, 2, 7, ROUGH_PRISMARINE);
            piece.setBlockState(worldIn, sBBIn, 0, 2, 6, ROUGH_PRISMARINE);
            piece.setBlockState(worldIn, sBBIn, 6, 2, 7, ROUGH_PRISMARINE);
            piece.setBlockState(worldIn, sBBIn, 7, 2, 6, ROUGH_PRISMARINE);
            piece.setBlockState(worldIn, sBBIn, 6, 2, 0, ROUGH_PRISMARINE);
            piece.setBlockState(worldIn, sBBIn, 7, 2, 1, ROUGH_PRISMARINE);

            if (!piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::SOUTH)]) {
                piece.fillWithBlocks(worldIn, sBBIn, 1, 3, 0, 6, 3, 0, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 1, 2, 0, 6, 2, 0, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 1, 1, 0, 6, 1, 0, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            }

            if (!piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::NORTH)]) {
                piece.fillWithBlocks(worldIn, sBBIn, 1, 3, 7, 6, 3, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 1, 2, 7, 6, 2, 7, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 1, 1, 7, 6, 1, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            }

            if (!piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::WEST)]) {
                piece.fillWithBlocks(worldIn, sBBIn, 0, 3, 1, 0, 3, 6, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 0, 2, 1, 0, 2, 6, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 0, 1, 1, 0, 1, 6, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            }

            if (!piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::EAST)]) {
                piece.fillWithBlocks(worldIn, sBBIn, 7, 3, 1, 7, 3, 6, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 7, 2, 1, 7, 2, 6, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 7, 1, 1, 7, 1, 6, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            }
        } else if (piece.m_data == 2) {
            piece.fillWithBlocks(worldIn, sBBIn, 0, 1, 0, 0, 1, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 7, 1, 0, 7, 1, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 1, 0, 6, 1, 0, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 1, 7, 6, 1, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 0, 2, 0, 0, 2, 7, DARK_PRISMARINE, DARK_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 7, 2, 0, 7, 2, 7, DARK_PRISMARINE, DARK_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 2, 0, 6, 2, 0, DARK_PRISMARINE, DARK_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 2, 7, 6, 2, 7, DARK_PRISMARINE, DARK_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 0, 3, 0, 0, 3, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 7, 3, 0, 7, 3, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 3, 0, 6, 3, 0, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 3, 7, 6, 3, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 0, 1, 3, 0, 2, 4, DARK_PRISMARINE, DARK_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 7, 1, 3, 7, 2, 4, DARK_PRISMARINE, DARK_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 3, 1, 0, 4, 2, 0, DARK_PRISMARINE, DARK_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 3, 1, 7, 4, 2, 7, DARK_PRISMARINE, DARK_PRISMARINE, false);

            if (piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::SOUTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 3, 1, 0, 4, 2, 0, false);
            }

            if (piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::NORTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 3, 1, 7, 4, 2, 7, false);
            }

            if (piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::WEST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 0, 1, 3, 0, 2, 4, false);
            }

            if (piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::EAST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 7, 1, 3, 7, 2, 4, false);
            }
        }

        if (flag) {
            piece.fillWithBlocks(worldIn, sBBIn, 3, 1, 3, 4, 1, 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 3, 2, 3, 4, 2, 4, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 3, 3, 3, 4, 3, 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        }

        return true;
    }
};

    class SimpleTopRoom : public OceanMonumentPiece {
public:
    SimpleTopRoom() = delete;

    SimpleTopRoom(EnumFacing facing, RoomDefinition& definition, RNG& rng)
        : OceanMonumentPiece(PT_Monument_SimpleTopRoom, facing, &definition, 1, 1, 1) {
    }

    static bool addComponentParts(World& worldIn, RNG& rng, BoundingBox sBBIn, OceanMonumentPiece& piece)  {
        if (piece.roomDefinition->index / 25 > 0) {
            piece.generateDefaultFloor(worldIn, sBBIn, 0, 0,
                                       piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::DOWN)]);
        }

        if (piece.roomDefinition->connections[getIndexFacing(EnumFacing::UP)] == nullptr) {
            piece.generateBoxOnFillOnly(worldIn, sBBIn, 1, 4, 1, 6, 4, 6, ROUGH_PRISMARINE);
        }

        for (int i = 1; i <= 6; ++i) {
            for (int j = 1; j <= 6; ++j) {
                if (rng.nextInt(3) != 0) {
                    int k = 2 + (rng.nextInt(4) == 0 ? 0 : 1);
                    piece.fillWithBlocks(worldIn, sBBIn, i, k, j, i, 3, j, lce::BlocksInit::SPONGE.getStateFromMeta(1),
                                         lce::BlocksInit::SPONGE.getStateFromMeta(1), false);
                }
            }
        }

        piece.fillWithBlocks(worldIn, sBBIn, 0, 1, 0, 0, 1, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 7, 1, 0, 7, 1, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 1, 1, 0, 6, 1, 0, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 1, 1, 7, 6, 1, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 0, 2, 0, 0, 2, 7, DARK_PRISMARINE, DARK_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 7, 2, 0, 7, 2, 7, DARK_PRISMARINE, DARK_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 1, 2, 0, 6, 2, 0, DARK_PRISMARINE, DARK_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 1, 2, 7, 6, 2, 7, DARK_PRISMARINE, DARK_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 0, 3, 0, 0, 3, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 7, 3, 0, 7, 3, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 1, 3, 0, 6, 3, 0, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 1, 3, 7, 6, 3, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 0, 1, 3, 0, 2, 4, DARK_PRISMARINE, DARK_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 7, 1, 3, 7, 2, 4, DARK_PRISMARINE, DARK_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 3, 1, 0, 4, 2, 0, DARK_PRISMARINE, DARK_PRISMARINE, false);
        piece.fillWithBlocks(worldIn, sBBIn, 3, 1, 7, 4, 2, 7, DARK_PRISMARINE, DARK_PRISMARINE, false);

        if (piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::SOUTH)]) {
            piece.generateWaterBox(worldIn, sBBIn, 3, 1, 0, 4, 2, 0, false);
        }

        return true;
    }
};

    class WingRoom : public OceanMonumentPiece {
    private:

    public:
        WingRoom() = delete;

        WingRoom(EnumFacing facing, BoundingBox bbIn, int mainDesign)
            : OceanMonumentPiece(facing, bbIn) {
            this->m_type = PT_Monument_WingRoom;
            this->m_data = mainDesign & 1;
        }

        static bool addComponentParts(World& worldIn, RNG& rng, BoundingBox sBBIn, OceanMonumentPiece& piece)  {
            if (piece.m_data == 0) {
                for (int i = 0; i < 4; ++i) {
                    piece.fillWithBlocks(worldIn, sBBIn, 10 - i, 3 - i, 20 - i, 12 + i, 3 - i, 20, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                }

                piece.fillWithBlocks(worldIn, sBBIn, 7, 0, 6, 15, 0, 16, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 6, 0, 6, 6, 3, 20, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 16, 0, 6, 16, 3, 20, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 7, 1, 7, 7, 1, 20, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 15, 1, 7, 15, 1, 20, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 7, 1, 6, 9, 3, 6, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 13, 1, 6, 15, 3, 6, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 8, 1, 7, 9, 1, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 13, 1, 7, 14, 1, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 9, 0, 5, 13, 0, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 10, 0, 7, 12, 0, 7, DARK_PRISMARINE, DARK_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 8, 0, 10, 8, 0, 12, DARK_PRISMARINE, DARK_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 14, 0, 10, 14, 0, 12, DARK_PRISMARINE, DARK_PRISMARINE, false);

                for (int i1 = 18; i1 >= 7; i1 -= 3) {
                    piece.setBlockState(worldIn, sBBIn, 6, 3, i1, SEA_LANTERN);
                    piece.setBlockState(worldIn, sBBIn, 16, 3, i1, SEA_LANTERN);
                }

                piece.setBlockState(worldIn, sBBIn, 10, 0, 10, SEA_LANTERN);
                piece.setBlockState(worldIn, sBBIn, 12, 0, 10, SEA_LANTERN);
                piece.setBlockState(worldIn, sBBIn, 10, 0, 12, SEA_LANTERN);
                piece.setBlockState(worldIn, sBBIn, 12, 0, 12, SEA_LANTERN);
                piece.setBlockState(worldIn, sBBIn, 8, 3, 6, SEA_LANTERN);
                piece.setBlockState(worldIn, sBBIn, 14, 3, 6, SEA_LANTERN);
                piece.setBlockState(worldIn, sBBIn, 4, 2, 4, BRICKS_PRISMARINE);
                piece.setBlockState(worldIn, sBBIn, 4, 1, 4, SEA_LANTERN);
                piece.setBlockState(worldIn, sBBIn, 4, 0, 4, BRICKS_PRISMARINE);
                piece.setBlockState(worldIn, sBBIn, 18, 2, 4, BRICKS_PRISMARINE);
                piece.setBlockState(worldIn, sBBIn, 18, 1, 4, SEA_LANTERN);
                piece.setBlockState(worldIn, sBBIn, 18, 0, 4, BRICKS_PRISMARINE);
                piece.setBlockState(worldIn, sBBIn, 4, 2, 18, BRICKS_PRISMARINE);
                piece.setBlockState(worldIn, sBBIn, 4, 1, 18, SEA_LANTERN);
                piece.setBlockState(worldIn, sBBIn, 4, 0, 18, BRICKS_PRISMARINE);
                piece.setBlockState(worldIn, sBBIn, 18, 2, 18, BRICKS_PRISMARINE);
                piece.setBlockState(worldIn, sBBIn, 18, 1, 18, SEA_LANTERN);
                piece.setBlockState(worldIn, sBBIn, 18, 0, 18, BRICKS_PRISMARINE);
                piece.setBlockState(worldIn, sBBIn, 9, 7, 20, BRICKS_PRISMARINE);
                piece.setBlockState(worldIn, sBBIn, 13, 7, 20, BRICKS_PRISMARINE);
                piece.fillWithBlocks(worldIn, sBBIn, 6, 0, 21, 7, 4, 21, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 15, 0, 21, 16, 4, 21, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                // piece.spawnElder(worldIn, sBBIn, 11, 2, 16);
            } else if (piece.m_data == 1) {
                piece.fillWithBlocks(worldIn, sBBIn, 9, 3, 18, 13, 3, 20, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 9, 0, 18, 9, 2, 18, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 13, 0, 18, 13, 2, 18, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                int j1 = 9;
                int j = 20;
                int k = 5;

                for (int l = 0; l < 2; ++l) {
                    piece.setBlockState(worldIn, sBBIn, j1, 6, 20, BRICKS_PRISMARINE);
                    piece.setBlockState(worldIn, sBBIn, j1, 5, 20, SEA_LANTERN);
                    piece.setBlockState(worldIn, sBBIn, j1, 4, 20, BRICKS_PRISMARINE);
                    j1 = 13;
                }

                piece.fillWithBlocks(worldIn, sBBIn, 7, 3, 7, 15, 3, 14, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                j1 = 10;

                for (int k1 = 0; k1 < 2; ++k1) {
                    piece.fillWithBlocks(worldIn, sBBIn, j1, 0, 10, j1, 6, 10, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    piece.fillWithBlocks(worldIn, sBBIn, j1, 0, 12, j1, 6, 12, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    piece.setBlockState(worldIn, sBBIn, j1, 0, 10, SEA_LANTERN);
                    piece.setBlockState(worldIn, sBBIn, j1, 0, 12, SEA_LANTERN);
                    piece.setBlockState(worldIn, sBBIn, j1, 4, 10, SEA_LANTERN);
                    piece.setBlockState(worldIn, sBBIn, j1, 4, 12, SEA_LANTERN);
                    j1 = 12;
                }

                j1 = 8;

                for (int l1 = 0; l1 < 2; ++l1) {
                    piece.fillWithBlocks(worldIn, sBBIn, j1, 0, 7, j1, 2, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    piece.fillWithBlocks(worldIn, sBBIn, j1, 0, 14, j1, 2, 14, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    j1 = 14;
                }

                piece.fillWithBlocks(worldIn, sBBIn, 8, 3, 8, 8, 3, 13, DARK_PRISMARINE, DARK_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 14, 3, 8, 14, 3, 13, DARK_PRISMARINE, DARK_PRISMARINE, false);
                // piece.spawnElder(worldIn, sBBIn, 11, 5, 13);
            }

            return true;
        }
    };

    class DoubleXRoom : public OceanMonumentPiece {
    public:
        DoubleXRoom() = delete;

        DoubleXRoom(EnumFacing facing, RoomDefinition& definition, RNG& rng)
            : OceanMonumentPiece(PT_Monument_DoubleXRoom, facing, &definition, 2, 1, 1) {}

        static bool addComponentParts(World& worldIn, RNG& rng, BoundingBox sBBIn, OceanMonumentPiece& piece)  {
            RoomDefinition* sop$rm =
                    piece.roomDefinition->connections[getIndexFacing(EnumFacing::EAST)];
            RoomDefinition* sop$rm1 = piece.roomDefinition;

            if (piece.roomDefinition->index / 25 > 0) {
                piece.generateDefaultFloor(
                        worldIn, sBBIn, 8, 0,
                        sop$rm->hasOpening[getIndexFacing(EnumFacing::DOWN)]);
                piece.generateDefaultFloor(
                        worldIn, sBBIn, 0, 0,
                        sop$rm1->hasOpening[getIndexFacing(EnumFacing::DOWN)]);
            }

            if (sop$rm1->connections[getIndexFacing(EnumFacing::UP)] == nullptr) {
                piece.generateBoxOnFillOnly(worldIn, sBBIn, 1, 4, 1, 7, 4, 6, ROUGH_PRISMARINE);
            }

            if (sop$rm->connections[getIndexFacing(EnumFacing::UP)] == nullptr) {
                piece.generateBoxOnFillOnly(worldIn, sBBIn, 8, 4, 1, 14, 4, 6, ROUGH_PRISMARINE);
            }

            piece.fillWithBlocks(worldIn, sBBIn, 0, 3, 0, 0, 3, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 15, 3, 0, 15, 3, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 3, 0, 15, 3, 0, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 3, 7, 14, 3, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 0, 2, 0, 0, 2, 7, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 15, 2, 0, 15, 2, 7, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 2, 0, 15, 2, 0, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 2, 7, 14, 2, 7, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 0, 1, 0, 0, 1, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 15, 1, 0, 15, 1, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 1, 0, 15, 1, 0, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 1, 7, 14, 1, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 1, 0, 10, 1, 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 6, 2, 0, 9, 2, 3, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 3, 0, 10, 3, 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.setBlockState(worldIn, sBBIn, 6, 2, 3, SEA_LANTERN);
            piece.setBlockState(worldIn, sBBIn, 9, 2, 3, SEA_LANTERN);

            if (sop$rm1->hasOpening[getIndexFacing(EnumFacing::SOUTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 3, 1, 0, 4, 2, 0, false);
            }

            if (sop$rm1->hasOpening[getIndexFacing(EnumFacing::NORTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 3, 1, 7, 4, 2, 7, false);
            }

            if (sop$rm1->hasOpening[getIndexFacing(EnumFacing::WEST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 0, 1, 3, 0, 2, 4, false);
            }

            if (sop$rm->hasOpening[getIndexFacing(EnumFacing::SOUTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 11, 1, 0, 12, 2, 0, false);
            }

            if (sop$rm->hasOpening[getIndexFacing(EnumFacing::NORTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 11, 1, 7, 12, 2, 7, false);
            }

            if (sop$rm->hasOpening[getIndexFacing(EnumFacing::EAST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 15, 1, 3, 15, 2, 4, false);
            }

            return true;
        }
    };

    class DoubleXYRoom : public OceanMonumentPiece {
    public:
        DoubleXYRoom() = delete;

        DoubleXYRoom(EnumFacing facing, RoomDefinition& definition, RNG& rng)
            : OceanMonumentPiece(PT_Monument_DoubleXYRoom, facing, &definition, 2, 2, 1) {}

        static bool addComponentParts(World& worldIn, RNG& rng, BoundingBox sBBIn, OceanMonumentPiece& piece)  {
            RoomDefinition* sop$rm =
                    piece.roomDefinition->connections[getIndexFacing(EnumFacing::EAST)];
            RoomDefinition* sop$rm1 = piece.roomDefinition;
            RoomDefinition* sop$rm2 =
                    sop$rm1->connections[getIndexFacing(EnumFacing::UP)];
            RoomDefinition* sop$rm3 =
                    sop$rm->connections[getIndexFacing(EnumFacing::UP)];

            if (piece.roomDefinition->index / 25 > 0) {
                piece.generateDefaultFloor(
                        worldIn, sBBIn, 8, 0,
                        sop$rm->hasOpening[getIndexFacing(EnumFacing::DOWN)]);
                piece.generateDefaultFloor(
                        worldIn, sBBIn, 0, 0,
                        sop$rm1->hasOpening[getIndexFacing(EnumFacing::DOWN)]);
            }

            if (sop$rm2->connections[getIndexFacing(EnumFacing::UP)] == nullptr) {
                piece.generateBoxOnFillOnly(worldIn, sBBIn, 1, 8, 1, 7, 8, 6, ROUGH_PRISMARINE);
            }

            if (sop$rm3->connections[getIndexFacing(EnumFacing::UP)] == nullptr) {
                piece.generateBoxOnFillOnly(worldIn, sBBIn, 8, 8, 1, 14, 8, 6, ROUGH_PRISMARINE);
            }

            for (int i = 1; i <= 7; ++i) {
                lce::BlockState iblockstate = BRICKS_PRISMARINE;

                if (i == 2 || i == 6) { iblockstate = ROUGH_PRISMARINE; }

                piece.fillWithBlocks(worldIn, sBBIn, 0, i, 0, 0, i, 7, iblockstate, iblockstate, false);
                piece.fillWithBlocks(worldIn, sBBIn, 15, i, 0, 15, i, 7, iblockstate, iblockstate, false);
                piece.fillWithBlocks(worldIn, sBBIn, 1, i, 0, 15, i, 0, iblockstate, iblockstate, false);
                piece.fillWithBlocks(worldIn, sBBIn, 1, i, 7, 14, i, 7, iblockstate, iblockstate, false);
            }

            piece.fillWithBlocks(worldIn, sBBIn, 2, 1, 3, 2, 7, 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 3, 1, 2, 4, 7, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 3, 1, 5, 4, 7, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 13, 1, 3, 13, 7, 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 11, 1, 2, 12, 7, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 11, 1, 5, 12, 7, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 1, 3, 5, 3, 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 10, 1, 3, 10, 3, 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 7, 2, 10, 7, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 5, 2, 5, 7, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 10, 5, 2, 10, 7, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 5, 5, 5, 7, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 10, 5, 5, 10, 7, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.setBlockState(worldIn, sBBIn, 6, 6, 2, BRICKS_PRISMARINE);
            piece.setBlockState(worldIn, sBBIn, 9, 6, 2, BRICKS_PRISMARINE);
            piece.setBlockState(worldIn, sBBIn, 6, 6, 5, BRICKS_PRISMARINE);
            piece.setBlockState(worldIn, sBBIn, 9, 6, 5, BRICKS_PRISMARINE);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 4, 3, 6, 4, 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 9, 4, 3, 10, 4, 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.setBlockState(worldIn, sBBIn, 5, 4, 2, SEA_LANTERN);
            piece.setBlockState(worldIn, sBBIn, 5, 4, 5, SEA_LANTERN);
            piece.setBlockState(worldIn, sBBIn, 10, 4, 2, SEA_LANTERN);
            piece.setBlockState(worldIn, sBBIn, 10, 4, 5, SEA_LANTERN);

            if (sop$rm1->hasOpening[getIndexFacing(EnumFacing::SOUTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 3, 1, 0, 4, 2, 0, false);
            }

            if (sop$rm1->hasOpening[getIndexFacing(EnumFacing::NORTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 3, 1, 7, 4, 2, 7, false);
            }

            if (sop$rm1->hasOpening[getIndexFacing(EnumFacing::WEST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 0, 1, 3, 0, 2, 4, false);
            }

            if (sop$rm->hasOpening[getIndexFacing(EnumFacing::SOUTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 11, 1, 0, 12, 2, 0, false);
            }

            if (sop$rm->hasOpening[getIndexFacing(EnumFacing::NORTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 11, 1, 7, 12, 2, 7, false);
            }

            if (sop$rm->hasOpening[getIndexFacing(EnumFacing::EAST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 15, 1, 3, 15, 2, 4, false);
            }

            if (sop$rm2->hasOpening[getIndexFacing(EnumFacing::SOUTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 3, 5, 0, 4, 6, 0, false);
            }

            if (sop$rm2->hasOpening[getIndexFacing(EnumFacing::NORTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 3, 5, 7, 4, 6, 7, false);
            }

            if (sop$rm2->hasOpening[getIndexFacing(EnumFacing::WEST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 0, 5, 3, 0, 6, 4, false);
            }

            if (sop$rm3->hasOpening[getIndexFacing(EnumFacing::SOUTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 11, 5, 0, 12, 6, 0, false);
            }

            if (sop$rm3->hasOpening[getIndexFacing(EnumFacing::NORTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 11, 5, 7, 12, 6, 7, false);
            }

            if (sop$rm3->hasOpening[getIndexFacing(EnumFacing::EAST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 15, 5, 3, 15, 6, 4, false);
            }

            return true;
        }
    };

    class DoubleYRoom : public OceanMonumentPiece {
    public:
        DoubleYRoom() = delete;

        DoubleYRoom(EnumFacing facing, RoomDefinition& definition, RNG& rng)
            : OceanMonumentPiece(PT_Monument_DoubleYRoom, facing, &definition, 1, 2, 1) {
        }

        static bool addComponentParts(World& worldIn, RNG& rng, BoundingBox sBBIn, OceanMonumentPiece& piece)  {
            if (piece.roomDefinition->index / 25 > 0) {
                piece.generateDefaultFloor(worldIn, sBBIn, 0, 0,
                                           piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::DOWN)]);
            }

            RoomDefinition* sop$rm =
                    piece.roomDefinition->connections[getIndexFacing(EnumFacing::UP)];

            if (sop$rm->connections[getIndexFacing(EnumFacing::UP)] == nullptr) {
                piece.generateBoxOnFillOnly(worldIn, sBBIn, 1, 8, 1, 6, 8, 6, ROUGH_PRISMARINE);
            }

            piece.fillWithBlocks(worldIn, sBBIn, 0, 4, 0, 0, 4, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 7, 4, 0, 7, 4, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 4, 0, 6, 4, 0, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 4, 7, 6, 4, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 2, 4, 1, 2, 4, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 4, 2, 1, 4, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 4, 1, 5, 4, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 6, 4, 2, 6, 4, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 2, 4, 5, 2, 4, 6, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 4, 5, 1, 4, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 4, 5, 5, 4, 6, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 6, 4, 5, 6, 4, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            RoomDefinition* sop$rm1 = piece.roomDefinition;

            for (int i = 1; i <= 5; i += 4) {
                int j = 0;

                if (sop$rm1->hasOpening[getIndexFacing(EnumFacing::SOUTH)]) {
                    piece.fillWithBlocks(worldIn, sBBIn, 2, i, j, 2, i + 2, j, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    piece.fillWithBlocks(worldIn, sBBIn, 5, i, j, 5, i + 2, j, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    piece.fillWithBlocks(worldIn, sBBIn, 3, i + 2, j, 4, i + 2, j, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                } else {
                    piece.fillWithBlocks(worldIn, sBBIn, 0, i, j, 7, i + 2, j, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    piece.fillWithBlocks(worldIn, sBBIn, 0, i + 1, j, 7, i + 1, j, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                }

                j = 7;

                if (sop$rm1->hasOpening[getIndexFacing(EnumFacing::NORTH)]) {
                    piece.fillWithBlocks(worldIn, sBBIn, 2, i, j, 2, i + 2, j, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    piece.fillWithBlocks(worldIn, sBBIn, 5, i, j, 5, i + 2, j, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    piece.fillWithBlocks(worldIn, sBBIn, 3, i + 2, j, 4, i + 2, j, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                } else {
                    piece.fillWithBlocks(worldIn, sBBIn, 0, i, j, 7, i + 2, j, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    piece.fillWithBlocks(worldIn, sBBIn, 0, i + 1, j, 7, i + 1, j, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                }

                int k = 0;

                if (sop$rm1->hasOpening[getIndexFacing(EnumFacing::WEST)]) {
                    piece.fillWithBlocks(worldIn, sBBIn, k, i, 2, k, i + 2, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    piece.fillWithBlocks(worldIn, sBBIn, k, i, 5, k, i + 2, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    piece.fillWithBlocks(worldIn, sBBIn, k, i + 2, 3, k, i + 2, 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                } else {
                    piece.fillWithBlocks(worldIn, sBBIn, k, i, 0, k, i + 2, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    piece.fillWithBlocks(worldIn, sBBIn, k, i + 1, 0, k, i + 1, 7, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                }

                k = 7;

                if (sop$rm1->hasOpening[getIndexFacing(EnumFacing::EAST)]) {
                    piece.fillWithBlocks(worldIn, sBBIn, k, i, 2, k, i + 2, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    piece.fillWithBlocks(worldIn, sBBIn, k, i, 5, k, i + 2, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    piece.fillWithBlocks(worldIn, sBBIn, k, i + 2, 3, k, i + 2, 4, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                } else {
                    piece.fillWithBlocks(worldIn, sBBIn, k, i, 0, k, i + 2, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                    piece.fillWithBlocks(worldIn, sBBIn, k, i + 1, 0, k, i + 1, 7, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
                }

                sop$rm1 = sop$rm;
            }

            return true;
        }
    };

    class DoubleYZRoom : public OceanMonumentPiece {
    public:
        DoubleYZRoom() = delete;

        DoubleYZRoom(EnumFacing facing, RoomDefinition& definition, RNG& rng)
            : OceanMonumentPiece(PT_Monument_DoubleYZRoom, facing, &definition, 1, 2, 2) {
        }

        static bool addComponentParts(World& worldIn, RNG& rng, BoundingBox sBBIn, OceanMonumentPiece& piece)  {
            RoomDefinition* sop$rm =
                    piece.roomDefinition->connections[getIndexFacing(EnumFacing::NORTH)];
            RoomDefinition* sop$rm1 = piece.roomDefinition;
            RoomDefinition* sop$rm2 =
                    sop$rm->connections[getIndexFacing(EnumFacing::UP)];
            RoomDefinition* sop$rm3 =
                    sop$rm1->connections[getIndexFacing(EnumFacing::UP)];

            if (piece.roomDefinition->index / 25 > 0) {
                piece.generateDefaultFloor(
                        worldIn, sBBIn, 0, 8,
                        sop$rm->hasOpening[getIndexFacing(EnumFacing::DOWN)]);
                piece.generateDefaultFloor(
                        worldIn, sBBIn, 0, 0,
                        sop$rm1->hasOpening[getIndexFacing(EnumFacing::DOWN)]);
            }

            if (sop$rm3->connections[getIndexFacing(EnumFacing::UP)] == nullptr) {
                piece.generateBoxOnFillOnly(worldIn, sBBIn, 1, 8, 1, 6, 8, 7, ROUGH_PRISMARINE);
            }

            if (sop$rm2->connections[getIndexFacing(EnumFacing::UP)] == nullptr) {
                piece.generateBoxOnFillOnly(worldIn, sBBIn, 1, 8, 8, 6, 8, 14, ROUGH_PRISMARINE);
            }

            for (int i = 1; i <= 7; ++i) {
                lce::BlockState iblockstate = BRICKS_PRISMARINE;

                if (i == 2 || i == 6) { iblockstate = ROUGH_PRISMARINE; }

                piece.fillWithBlocks(worldIn, sBBIn, 0, i, 0, 0, i, 15, iblockstate, iblockstate, false);
                piece.fillWithBlocks(worldIn, sBBIn, 7, i, 0, 7, i, 15, iblockstate, iblockstate, false);
                piece.fillWithBlocks(worldIn, sBBIn, 1, i, 0, 6, i, 0, iblockstate, iblockstate, false);
                piece.fillWithBlocks(worldIn, sBBIn, 1, i, 15, 6, i, 15, iblockstate, iblockstate, false);
            }

            for (int j = 1; j <= 7; ++j) {
                lce::BlockState iblockstate1 = DARK_PRISMARINE;

                if (j == 2 || j == 6) { iblockstate1 = SEA_LANTERN; }

                piece.fillWithBlocks(worldIn, sBBIn, 3, j, 7, 4, j, 8, iblockstate1, iblockstate1, false);
            }

            if (sop$rm1->hasOpening[getIndexFacing(EnumFacing::SOUTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 3, 1, 0, 4, 2, 0, false);
            }

            if (sop$rm1->hasOpening[getIndexFacing(EnumFacing::EAST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 7, 1, 3, 7, 2, 4, false);
            }

            if (sop$rm1->hasOpening[getIndexFacing(EnumFacing::WEST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 0, 1, 3, 0, 2, 4, false);
            }

            if (sop$rm->hasOpening[getIndexFacing(EnumFacing::NORTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 3, 1, 15, 4, 2, 15, false);
            }

            if (sop$rm->hasOpening[getIndexFacing(EnumFacing::WEST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 0, 1, 11, 0, 2, 12, false);
            }

            if (sop$rm->hasOpening[getIndexFacing(EnumFacing::EAST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 7, 1, 11, 7, 2, 12, false);
            }

            if (sop$rm3->hasOpening[getIndexFacing(EnumFacing::SOUTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 3, 5, 0, 4, 6, 0, false);
            }

            if (sop$rm3->hasOpening[getIndexFacing(EnumFacing::EAST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 7, 5, 3, 7, 6, 4, false);
                piece.fillWithBlocks(worldIn, sBBIn, 5, 4, 2, 6, 4, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 6, 1, 2, 6, 3, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 6, 1, 5, 6, 3, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            }

            if (sop$rm3->hasOpening[getIndexFacing(EnumFacing::WEST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 0, 5, 3, 0, 6, 4, false);
                piece.fillWithBlocks(worldIn, sBBIn, 1, 4, 2, 2, 4, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 1, 1, 2, 1, 3, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 1, 1, 5, 1, 3, 5, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            }

            if (sop$rm2->hasOpening[getIndexFacing(EnumFacing::NORTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 3, 5, 15, 4, 6, 15, false);
            }

            if (sop$rm2->hasOpening[getIndexFacing(EnumFacing::WEST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 0, 5, 11, 0, 6, 12, false);
                piece.fillWithBlocks(worldIn, sBBIn, 1, 4, 10, 2, 4, 13, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 1, 1, 10, 1, 3, 10, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 1, 1, 13, 1, 3, 13, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            }

            if (sop$rm2->hasOpening[getIndexFacing(EnumFacing::EAST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 7, 5, 11, 7, 6, 12, false);
                piece.fillWithBlocks(worldIn, sBBIn, 5, 4, 10, 6, 4, 13, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 6, 1, 10, 6, 3, 10, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
                piece.fillWithBlocks(worldIn, sBBIn, 6, 1, 13, 6, 3, 13, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            }

            return true;
        }
    };

    class DoubleZRoom : public OceanMonumentPiece {
    public:
        DoubleZRoom() = delete;

        DoubleZRoom(EnumFacing facing, RoomDefinition& definition, RNG& rng)
            : OceanMonumentPiece(PT_Monument_DoubleZRoom, facing, &definition, 1, 1, 2) {
        }

        static bool addComponentParts(World& worldIn, RNG& rng, BoundingBox sBBIn, OceanMonumentPiece& piece) {
            RoomDefinition* sop$rm =
                    piece.roomDefinition->connections[getIndexFacing(EnumFacing::NORTH)];
            RoomDefinition* sop$rm1 = piece.roomDefinition;

            if (piece.roomDefinition->index / 25 > 0) {
                piece.generateDefaultFloor(
                        worldIn, sBBIn, 0, 8,
                        sop$rm->hasOpening[getIndexFacing(EnumFacing::DOWN)]);
                piece.generateDefaultFloor(
                        worldIn, sBBIn, 0, 0,
                        sop$rm1->hasOpening[getIndexFacing(EnumFacing::DOWN)]);
            }

            if (sop$rm1->connections[getIndexFacing(EnumFacing::UP)] == nullptr) {
                piece.generateBoxOnFillOnly(worldIn, sBBIn, 1, 4, 1, 6, 4, 7, ROUGH_PRISMARINE);
            }

            if (sop$rm->connections[getIndexFacing(EnumFacing::UP)] == nullptr) {
                piece.generateBoxOnFillOnly(worldIn, sBBIn, 1, 4, 8, 6, 4, 14, ROUGH_PRISMARINE);
            }

            piece.fillWithBlocks(worldIn, sBBIn, 0, 3, 0, 0, 3, 15, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 7, 3, 0, 7, 3, 15, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 3, 0, 7, 3, 0, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 3, 15, 6, 3, 15, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 0, 2, 0, 0, 2, 15, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 7, 2, 0, 7, 2, 15, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 2, 0, 7, 2, 0, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 2, 15, 6, 2, 15, ROUGH_PRISMARINE, ROUGH_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 0, 1, 0, 0, 1, 15, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 7, 1, 0, 7, 1, 15, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 1, 0, 7, 1, 0, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 1, 15, 6, 1, 15, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 1, 1, 1, 1, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 6, 1, 1, 6, 1, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 3, 1, 1, 3, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 6, 3, 1, 6, 3, 2, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 1, 13, 1, 1, 14, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 6, 1, 13, 6, 1, 14, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 3, 13, 1, 3, 14, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 6, 3, 13, 6, 3, 14, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 2, 1, 6, 2, 3, 6, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 1, 6, 5, 3, 6, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 2, 1, 9, 2, 3, 9, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 1, 9, 5, 3, 9, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 3, 2, 6, 4, 2, 6, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 3, 2, 9, 4, 2, 9, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 2, 2, 7, 2, 2, 8, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 2, 7, 5, 2, 8, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.setBlockState(worldIn, sBBIn, 2, 2, 5, SEA_LANTERN);
            piece.setBlockState(worldIn, sBBIn, 5, 2, 5, SEA_LANTERN);
            piece.setBlockState(worldIn, sBBIn, 2, 2, 10, SEA_LANTERN);
            piece.setBlockState(worldIn, sBBIn, 5, 2, 10, SEA_LANTERN);
            piece.setBlockState(worldIn, sBBIn, 2, 3, 5, BRICKS_PRISMARINE);
            piece.setBlockState(worldIn, sBBIn, 5, 3, 5, BRICKS_PRISMARINE);
            piece.setBlockState(worldIn, sBBIn, 2, 3, 10, BRICKS_PRISMARINE);
            piece.setBlockState(worldIn, sBBIn, 5, 3, 10, BRICKS_PRISMARINE);

            if (sop$rm1->hasOpening[getIndexFacing(EnumFacing::SOUTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 3, 1, 0, 4, 2, 0, false);
            }

            if (sop$rm1->hasOpening[getIndexFacing(EnumFacing::EAST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 7, 1, 3, 7, 2, 4, false);
            }

            if (sop$rm1->hasOpening[getIndexFacing(EnumFacing::WEST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 0, 1, 3, 0, 2, 4, false);
            }

            if (sop$rm->hasOpening[getIndexFacing(EnumFacing::NORTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 3, 1, 15, 4, 2, 15, false);
            }

            if (sop$rm->hasOpening[getIndexFacing(EnumFacing::WEST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 0, 1, 11, 0, 2, 12, false);
            }

            if (sop$rm->hasOpening[getIndexFacing(EnumFacing::EAST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 7, 1, 11, 7, 2, 12, false);
            }

            return true;
        }
    };

    class EntryRoom : public OceanMonumentPiece {
    public:
        EntryRoom() = delete;

        EntryRoom(EnumFacing facing, RoomDefinition& definition)
            : OceanMonumentPiece(PT_Monument_EntryRoom, facing, &definition, 1, 1, 1) {
        }

        static bool addComponentParts(World& worldIn, RNG& rng, BoundingBox sBBIn, OceanMonumentPiece& piece)  {
            piece.fillWithBlocks(worldIn, sBBIn, 0, 3, 0, 2, 3, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 3, 0, 7, 3, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 0, 2, 0, 1, 2, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 6, 2, 0, 7, 2, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 0, 1, 0, 0, 1, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 7, 1, 0, 7, 1, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 0, 1, 7, 7, 3, 7, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 1, 1, 0, 2, 3, 0, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);
            piece.fillWithBlocks(worldIn, sBBIn, 5, 1, 0, 6, 3, 0, BRICKS_PRISMARINE, BRICKS_PRISMARINE, false);

            if (piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::NORTH)]) {
                piece.generateWaterBox(worldIn, sBBIn, 3, 1, 7, 4, 2, 7, false);
            }

            if (piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::WEST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 0, 1, 3, 1, 2, 4, false);
            }

            if (piece.roomDefinition->hasOpening[getIndexFacing(EnumFacing::EAST)]) {
                piece.generateWaterBox(worldIn, sBBIn, 6, 1, 3, 7, 2, 4, false);
            }

            return true;
        }
    };




    class FitSimpleRoomHelper : public MonumentRoomFitHelper {
    public:
        FitSimpleRoomHelper() = default;

        bool fits(const RoomDefinition& definition) override { return true; }

        OceanMonumentPiece create(EnumFacing facing, RoomDefinition& direction, RNG& rng) override {
            direction.claimed = true;
            return SimpleRoom(facing, direction, rng);
        }
    };

    class FitSimpleRoomTopHelper : public MonumentRoomFitHelper{
    public:
        FitSimpleRoomTopHelper()= default;

        bool fits(const RoomDefinition& definition) override{
            return !definition.hasOpening[getIndexFacing(EnumFacing::WEST)] &&
                   !definition.hasOpening[getIndexFacing(EnumFacing::EAST)] &&
                   !definition.hasOpening[getIndexFacing(EnumFacing::NORTH)] &&
                   !definition.hasOpening[getIndexFacing(EnumFacing::SOUTH)] &&
                   !definition.hasOpening[getIndexFacing(EnumFacing::UP)];
        }

        OceanMonumentPiece create(EnumFacing facing, RoomDefinition& definition, RNG& rng) override {
            definition.claimed = true;
            return SimpleTopRoom(facing, definition, rng);
        }
    };

    class XDoubleRoomFitHelper : public MonumentRoomFitHelper {
    public:
        XDoubleRoomFitHelper() = default;

        bool fits(const RoomDefinition& definition) override {
            return definition.hasOpening[getIndexFacing(EnumFacing::EAST)] &&
                   !definition.connections[getIndexFacing(EnumFacing::EAST)]->claimed;
        }

        OceanMonumentPiece create(EnumFacing facing, RoomDefinition& definition, RNG& rng) override {
            definition.claimed = true;
            definition.connections[getIndexFacing(EnumFacing::EAST)]->claimed = true;
            return DoubleXRoom(facing, definition, rng);
        }
    };

    class XYDoubleRoomFitHelper : public MonumentRoomFitHelper {
    public:
        XYDoubleRoomFitHelper() = default;

        bool fits(const RoomDefinition& definition) override {
            if (definition.hasOpening[getIndexFacing(EnumFacing::EAST)] &&
                !definition.connections[getIndexFacing(EnumFacing::EAST)]->claimed &&
                definition.hasOpening[getIndexFacing(EnumFacing::UP)] &&
                !definition.connections[getIndexFacing(EnumFacing::UP)]->claimed) {
                RoomDefinition* sop$rm =
                        definition.connections[getIndexFacing(EnumFacing::EAST)];
                return sop$rm->hasOpening[getIndexFacing(EnumFacing::UP)] &&
                       !sop$rm->connections[getIndexFacing(EnumFacing::UP)]->claimed;
            } else {
                return false;
            }
        }

        OceanMonumentPiece create(EnumFacing facing, RoomDefinition& definition, RNG& rng) override {
            definition.claimed = true;
            definition.connections[getIndexFacing(EnumFacing::EAST)]->claimed = true;
            definition.connections[getIndexFacing(EnumFacing::UP)]->claimed = true;
            definition.connections[getIndexFacing(EnumFacing::EAST)]->connections[getIndexFacing(EnumFacing::UP)]->claimed = true;
            return DoubleXYRoom(facing, definition, rng);
        }
    };

    class YDoubleRoomFitHelper : public MonumentRoomFitHelper {
    public:
        YDoubleRoomFitHelper() = default;

        bool fits(const RoomDefinition& definition) override {
            return definition.hasOpening[getIndexFacing(EnumFacing::UP)] &&
                   !definition.connections[getIndexFacing(EnumFacing::UP)]->claimed;
        }

        OceanMonumentPiece create(EnumFacing facing, RoomDefinition& definition, RNG& rng) override {
            definition.claimed = true;
            definition.connections[getIndexFacing(EnumFacing::UP)]->claimed = true;
            return DoubleYRoom(facing, definition, rng);
        }
    };

    class YZDoubleRoomFitHelper : public MonumentRoomFitHelper {
    public:
        YZDoubleRoomFitHelper() = default;

        bool fits(const RoomDefinition& definition) override {
            if (definition.hasOpening[getIndexFacing(EnumFacing::NORTH)] &&
                !definition.connections[getIndexFacing(EnumFacing::NORTH)]->claimed &&
                definition.hasOpening[getIndexFacing(EnumFacing::UP)] &&
                !definition.connections[getIndexFacing(EnumFacing::UP)]->claimed) {
                RoomDefinition* sop$rm =
                        definition.connections[getIndexFacing(EnumFacing::NORTH)];
                return sop$rm->hasOpening[getIndexFacing(EnumFacing::UP)] &&
                       !sop$rm->connections[getIndexFacing(EnumFacing::UP)]->claimed;
            } else {
                return false;
            }
        }

        OceanMonumentPiece create(EnumFacing facing, RoomDefinition& definition, RNG& rng) override {
            definition.claimed = true;
            definition.connections[getIndexFacing(EnumFacing::NORTH)]->claimed = true;
            definition.connections[getIndexFacing(EnumFacing::UP)]->claimed = true;
            definition.connections[getIndexFacing(EnumFacing::NORTH)]->connections[getIndexFacing(EnumFacing::UP)]->claimed = true;
            return DoubleYZRoom(facing, definition, rng);
        }
    };

    class ZDoubleRoomFitHelper : public MonumentRoomFitHelper {
    public:
        ZDoubleRoomFitHelper() = default;

        bool fits(const RoomDefinition& definition) override {
            return definition.hasOpening[getIndexFacing(EnumFacing::NORTH)] &&
                   !definition.connections[getIndexFacing(EnumFacing::NORTH)]->claimed;
        }

        OceanMonumentPiece create(EnumFacing facing, RoomDefinition& definition, RNG& rng) override {
            RoomDefinition* sop$rm = &definition;

            if (!definition.hasOpening[getIndexFacing(EnumFacing::NORTH)] ||
                definition.connections[getIndexFacing(EnumFacing::NORTH)]->claimed) {
                sop$rm = definition.connections[getIndexFacing(EnumFacing::SOUTH)];
            }

            sop$rm->claimed = true;
            sop$rm->connections[getIndexFacing(EnumFacing::NORTH)]->claimed = true;
            return DoubleZRoom(facing, *sop$rm, rng);
        }
    };
};
