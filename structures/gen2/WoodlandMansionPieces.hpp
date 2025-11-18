#pragma once
#include "StructureComponentTemplate.hpp"
#include "common/RotationAndMirror.hpp"
#include "common/rng.hpp"


#include <string>
#include <utility>
#include <list>


class WoodlandMansionPieces {

    
public: 
    static void registerPieces() {
        // MapGenStructureIO.registerStructureComponent(WoodlandMansionPieces::MansionTemplate*, "WMP");
    }
    
    static void generateAt(TemplateManager& templateManager, const Pos3D origin, const Rotation rotation, std::list<WoodlandMansionPieces::MansionTemplate> pieces, RNG& random) {
        Grid woodlandMansionPieces$grid = Grid(random);
        Placer woodlandMansionPieces$placer = Placer(templateManager, random);
        woodlandMansionPieces$placer.placeMansion(origin, rotation, pieces, woodlandMansionPieces$grid);
    }


    class SimpleGrid {
    public:
        std::vector<std::vector<int>> cells;
        const int width;
        const int height;
        const int defaultValue;

        SimpleGrid(const int widthIn, const int heightIn, const int defaultValue)
        : width(widthIn), height(heightIn), defaultValue(defaultValue) {
            for (int i = 0; i < widthIn; i++) {
                this->cells.emplace_back(heightIn);
            }
        }

        void set(int x, int z, int value) {
            if (x >= 0 && x < this->width && z >= 0 && z < this->height) {
                this->cells[x][z] = value;
            }
        }

        void fill(int x1, int z1, int x2, int z2, int value) {
            for (int i = z1; i <= z2; ++i) {
                for (int j = x1; j <= x2; ++j) {
                    this->set(j, i, value);
                }
            }
        }

        int get(int x, int z) {
            return x >= 0 && x < this->width && z >= 0 && z < this->height ? this->cells[x][z] : this->defaultValue;
        }

        void setIfEquals(int x, int z, int expected, int newValue) {
            if (this->get(x, z) == expected) {
                this->set(x, z, newValue);
            }
        }

        bool hasAdjacentOfType(int x, int z, int value) {
            return this->get(x - 1, z) == value
                || this->get(x + 1, z) == value
                || this->get(x, z + 1) == value
                || this->get(x, z - 1) == value;
        }
    };

    class RoomCollection {
    public:
        RoomCollection() = default;
        virtual ~RoomCollection() = default;
        virtual std::string get1x1Room(RNG& rand);
        virtual std::string get1x1Secret(RNG& rand);
        virtual std::string get1x2A(RNG& rand, bool withStairs);
        virtual std::string get1x2B(RNG& rand, bool withStairs);
        virtual std::string get1x2StairsEnd(RNG& rand);
        virtual std::string get2x2Room(RNG& rand);
        virtual std::string get2x2Secret(RNG& rand);

    protected:
        static char getIntChar(const int value) {
            static std::array<char, 8> CHARACTERS = {'0', '1', '2', '3', '4', '5', '6', '7'};
            return CHARACTERS[value];
        }
    };

    

    class FirstFloor final : public RoomCollection {

    public:
        FirstFloor() {}

        std::string get1x1Room(RNG& rand) override {
            const char value = getIntChar(rand.nextInt(5) + 1);
            return "1x1_a" + std::string(1, value);
        }

        std::string get1x1Secret(RNG& rand) override {
            const char value = getIntChar(rand.nextInt(4) + 1);
            return "1x1_as" + std::string(1, value);
        }

        std::string get1x2A(RNG& rand, bool withStairs) override {
            const char value = getIntChar(rand.nextInt(9) + 1);
            return "1x2_a" + std::string(1, value);
        }

        std::string get1x2B(RNG& rand, bool withStairs) override {
            const char value = getIntChar(rand.nextInt(5) + 1);
            return "1x2_b" + std::string(1, value);
        }

        std::string get1x2StairsEnd(RNG& rand) override {
            const char value = getIntChar(rand.nextInt(2) + 1);
            return "1x2_s" + std::string(1, value);
        }
        
        std::string get2x2Room(RNG& rand) override {
            const char value = getIntChar(rand.nextInt(4) + 1);
            return "2x2_a" + std::string(1, value);
        }

        std::string get2x2Secret(RNG& rand) override {
            return "2x2_s1";
        }
    };

    class Grid {
    public:

        /*const*/ RNG& random;
        /*const*/ SimpleGrid baseGrid;
        /*const*/ SimpleGrid upperGrid;
        /*const*/ std::vector<SimpleGrid> floorGrid;
        const int originX;
        const int originZ;

        Grid(RNG& random) : random(random),originX(7), originZ(4), baseGrid(11, 11, 5) {
            int i = 11;
            this->baseGrid.fill(this->originX, this->originZ, this->originX + 1, this->originZ + 1, 3);
            this->baseGrid.fill(this->originX - 1, this->originZ, this->originX - 1, this->originZ + 1, 2);
            this->baseGrid.fill(this->originX + 2, this->originZ - 2, this->originX + 3, this->originZ + 3, 5);
            this->baseGrid.fill(this->originX + 1, this->originZ - 2, this->originX + 1, this->originZ - 1, 1);
            this->baseGrid.fill(this->originX + 1, this->originZ + 2, this->originX + 1, this->originZ + 3, 1);
            this->baseGrid.set(this->originX - 1, this->originZ - 1, 1);
            this->baseGrid.set(this->originX - 1, this->originZ + 2, 1);
            this->baseGrid.fill(0, 0, 11, 1, 5);
            this->baseGrid.fill(0, 9, 11, 11, 5);
            this->carvePath(this->baseGrid, this->originX, this->originZ - 2, EnumFacing::WEST, 6);
            this->carvePath(this->baseGrid, this->originX, this->originZ + 3, EnumFacing::WEST, 6);
            this->carvePath(this->baseGrid, this->originX - 2, this->originZ - 1, EnumFacing::WEST, 3);
            this->carvePath(this->baseGrid, this->originX - 2, this->originZ + 2, EnumFacing::WEST, 3);

            while (this->expandJunctions(this->baseGrid)) {
                ;
            }

            this->floorGrid = std::vector<SimpleGrid>(3);
            this->floorGrid[0] = SimpleGrid(11, 11, 5);
            this->floorGrid[1] = SimpleGrid(11, 11, 5);
            this->floorGrid[2] = SimpleGrid(11, 11, 5);
            this->copyLayout(this->baseGrid, this->floorGrid[0]);
            this->copyLayout(this->baseGrid, this->floorGrid[1]);
            this->floorGrid[0].fill(this->originX + 1, this->originZ, this->originX + 1, this->originZ + 1, 8388608);
            this->floorGrid[1].fill(this->originX + 1, this->originZ, this->originX + 1, this->originZ + 1, 8388608);
            this->upperGrid = SimpleGrid(this->baseGrid.width, this->baseGrid.height, 5);
            this->planUpperFloor();
            this->copyLayout(this->upperGrid, this->floorGrid[2]);
        }
        
        static bool isRoomOrHall(SimpleGrid& grid, const int x, const int z) {
            int i = grid.get(x, z);
            return i == 1 || i == 2 || i == 3 || i == 4;
        }

        bool hasRoomId(SimpleGrid& _g, const int x, const int z, const int floorIdx, const int id) {
            return (this->floorGrid[floorIdx].get(x, z) & 65535) == id;
        }

        EnumFacing findNeighborWithId(SimpleGrid& g, int x, int z, const int floorIdx, const int id) {
            for (EnumFacing enumfacing : EnumFacing::Plane.HORIZONTAL.facings()) {
                if (this->hasRoomId(g, x + enumfacing.getFrontOffsetX(), z + enumfacing.getFrontOffsetZ(), floorIdx, id)) {
                    return enumfacing;
                }
            }

            return null;
        }

    private: 
        void carvePath(SimpleGrid& g, int x, int z, EnumFacing dir, const int depth) {
            if (depth > 0) {
                g.set(x, z, 1);
                g.setIfEquals(x + dir.getFrontOffsetX(), z + dir.getFrontOffsetZ(), 0, 1);

                for (int i = 0; i < 8; ++i) {
                    EnumFacing enumfacing = EnumFacing::getHorizontal(this->random.nextInt(4));

                    if (enumfacing != dir.getOpposite() && (enumfacing != EnumFacing::EAST || !this->random.nextBoolean())) {
                        int j = x + dir.getFrontOffsetX();
                        int k = z + dir.getFrontOffsetZ();

                        if (g.get(j + enumfacing.getFrontOffsetX(), k + enumfacing.getFrontOffsetZ()) == 0 && g.get(j + enumfacing.getFrontOffsetX() * 2, k + enumfacing.getFrontOffsetZ() * 2) == 0) {
                            this->carvePath(g, x + dir.getFrontOffsetX() + enumfacing.getFrontOffsetX(), z + dir.getFrontOffsetZ() + enumfacing.getFrontOffsetZ(), enumfacing, depth - 1);
                            break;
                        }
                    }
                }

                EnumFacing enumfacing1 = dir.rotateY();
                EnumFacing enumfacing2 = dir.rotateYCCW();
                g.setIfEquals(x + enumfacing1.getFrontOffsetX(), z + enumfacing1.getFrontOffsetZ(), 0, 2);
                g.setIfEquals(x + enumfacing2.getFrontOffsetX(), z + enumfacing2.getFrontOffsetZ(), 0, 2);
                g.setIfEquals(x + dir.getFrontOffsetX() + enumfacing1.getFrontOffsetX(), z + dir.getFrontOffsetZ() + enumfacing1.getFrontOffsetZ(), 0, 2);
                g.setIfEquals(x + dir.getFrontOffsetX() + enumfacing2.getFrontOffsetX(), z + dir.getFrontOffsetZ() + enumfacing2.getFrontOffsetZ(), 0, 2);
                g.setIfEquals(x + dir.getFrontOffsetX() * 2, z + dir.getFrontOffsetZ() * 2, 0, 2);
                g.setIfEquals(x + enumfacing1.getFrontOffsetX() * 2, z + enumfacing1.getFrontOffsetZ() * 2, 0, 2);
                g.setIfEquals(x + enumfacing2.getFrontOffsetX() * 2, z + enumfacing2.getFrontOffsetZ() * 2, 0, 2);
            }
        }

        bool expandJunctions(SimpleGrid& g) {
            bool flag = false;

            for (int i = 0; i < g.height; ++i) {
                for (int j = 0; j < g.width; ++j) {
                    if (g.get(j, i) == 0) {
                        int k = 0;
                        k = k + (isRoomOrHall(g, j + 1, i) ? 1 : 0);
                        k = k + (isRoomOrHall(g, j - 1, i) ? 1 : 0);
                        k = k + (isRoomOrHall(g, j, i + 1) ? 1 : 0);
                        k = k + (isRoomOrHall(g, j, i - 1) ? 1 : 0);

                        if (k >= 3) {
                            g.set(j, i, 2);
                            flag = true;
                        } else if (k == 2) {
                            int l = 0;
                            l = l + (isRoomOrHall(g, j + 1, i + 1) ? 1 : 0);
                            l = l + (isRoomOrHall(g, j - 1, i + 1) ? 1 : 0);
                            l = l + (isRoomOrHall(g, j + 1, i - 1) ? 1 : 0);
                            l = l + (isRoomOrHall(g, j - 1, i - 1) ? 1 : 0);

                            if (l <= 1) {
                                g.set(j, i, 2);
                                flag = true;
                            }
                        }
                    }
                }
            }

            return flag;
        }

        void planUpperFloor() {
            std::list<std::pair<int, int>> list = {};
            SimpleGrid woodlandMansionPieces$simpleGrid = this->floorGrid[1];

            for (int i = 0; i < this->upperGrid.height; ++i) {
                for (int j = 0; j < this->upperGrid.width; ++j) {
                    const int k = woodlandMansionPieces$simpleGrid.get(j, i);
                    const int l = k & 983040;

                    if (l == 131072 && (k & 2097152) == 2097152) {
                        list.add(std::make_pair(j, i));
                    }
                }
            }

            if (list.empty()) {
                this->upperGrid.fill(0, 0, this->upperGrid.width, this->upperGrid.height, 5);
            } else {
                std::pair<int, int> tuple = list.get(this->random.nextInt(list.size()));
                const int l1 = woodlandMansionPieces$simpleGrid.get(tuple.first, tuple.second);
                woodlandMansionPieces$simpleGrid.set(tuple.first, tuple.second, l1 | 4194304);
                EnumFacing enumFacing1 = this->findNeighborWithId(this->baseGrid, tuple.first, tuple.second, 1, l1 & 65535);
                int i2 = tuple.first + enumFacing1.getFrontOffsetX();
                int i1 = tuple.second + enumFacing1.getFrontOffsetZ();

                for (int j1 = 0; j1 < this->upperGrid.height; ++j1) {
                    for (int k1 = 0; k1 < this->upperGrid.width; ++k1) {
                        if (!isRoomOrHall(this->baseGrid, k1, j1)) {
                            this->upperGrid.set(k1, j1, 5);
                        } else if (k1 == tuple.first && j1 == tuple.second) {
                            this->upperGrid.set(k1, j1, 3);
                        } else if (k1 == i2 && j1 == i1) {
                            this->upperGrid.set(k1, j1, 3);
                            this->floorGrid[2].set(k1, j1, 8388608);
                        }
                    }
                }

                std::list<EnumFacing> list1 = {};

                for (EnumFacing enumfacing : EnumFacing::Plane.HORIZONTAL.facings()) {
                    if (this->upperGrid.get(i2 + enumfacing.getFrontOffsetX(), i1 + enumfacing.getFrontOffsetZ()) == 0) {
                        list1.add(enumfacing);
                    }
                }

                if (list1.empty()) {
                    this->upperGrid.fill(0, 0, this->upperGrid.width, this->upperGrid.height, 5);
                    woodlandMansionPieces$simpleGrid.set(tuple.first, tuple.second, l1);
                } else {
                    const EnumFacing enumFacing2 = list1.get(this->random.nextInt(list1.size()));
                    this->carvePath(this->upperGrid, i2 + enumFacing2.getFrontOffsetX(), i1 + enumFacing2.getFrontOffsetZ(), enumFacing2, 4);

                    while (this->expandJunctions(this->upperGrid)) {
                        ;
                    }
                }
            }
        }

        void copyLayout(SimpleGrid& src, SimpleGrid& dst) {
            std::list<std::pair<int, int>> list = {};

            for (int i = 0; i < src.height; ++i) {
                for (int j = 0; j < src.width; ++j) {
                    if (src.get(j, i) == 2) {
                        list.add(std::make_pair(j, i));
                    }
                }
            }

            Collections.shuffle(list, this->random);
            int k3 = 10;

            for (auto& [fst, snd] : list) {
                const int k = fst;
                const int l = snd;

                if (dst.get(k, l) == 0) {
                    int i1 = k;
                    int j1 = k;
                    int k1 = l;
                    int l1 = l;
                    int i2 = 65536;

                    if (dst.get(k + 1, l) == 0 && dst.get(k, l + 1) == 0 && dst.get(k + 1, l + 1) == 0 &&
                        src.get(k + 1, l) == 2 && src.get(k, l + 1) == 2 && src.get(k + 1, l + 1) == 2) {
                        j1 = k + 1;
                        l1 = l + 1;
                        i2 = 262144;
                    } else if (dst.get(k - 1, l) == 0 && dst.get(k, l + 1) == 0 && dst.get(k - 1, l + 1) == 0 &&
                               src.get(k - 1, l) == 2 && src.get(k, l + 1) == 2 && src.get(k - 1, l + 1) == 2) {
                        i1 = k - 1;
                        l1 = l + 1;
                        i2 = 262144;
                    } else if (dst.get(k - 1, l) == 0 && dst.get(k, l - 1) == 0 && dst.get(k - 1, l - 1) == 0 &&
                               src.get(k - 1, l) == 2 && src.get(k, l - 1) == 2 && src.get(k - 1, l - 1) == 2) {
                        i1 = k - 1;
                        k1 = l - 1;
                        i2 = 262144;
                    } else if (dst.get(k + 1, l) == 0 && src.get(k + 1, l) == 2) {
                        j1 = k + 1;
                        i2 = 131072;
                    } else if (dst.get(k, l + 1) == 0 && src.get(k, l + 1) == 2) {
                        l1 = l + 1;
                        i2 = 131072;
                    } else if (dst.get(k - 1, l) == 0 && src.get(k - 1, l) == 2) {
                        i1 = k - 1;
                        i2 = 131072;
                    } else if (dst.get(k, l - 1) == 0 && src.get(k, l - 1) == 2) {
                        k1 = l - 1;
                        i2 = 131072;
                    }

                    int j2 = this->random.nextBoolean() ? i1 : j1;
                    int k2 = this->random.nextBoolean() ? k1 : l1;
                    int l2 = 2097152;

                    if (!src.hasAdjacentOfType(j2, k2, 1)) {
                        j2 = j2 == i1 ? j1 : i1;
                        k2 = k2 == k1 ? l1 : k1;

                        if (!src.hasAdjacentOfType(j2, k2, 1)) {
                            k2 = k2 == k1 ? l1 : k1;

                            if (!src.hasAdjacentOfType(j2, k2, 1)) {
                                j2 = j2 == i1 ? j1 : i1;
                                k2 = k2 == k1 ? l1 : k1;

                                if (!src.hasAdjacentOfType(j2, k2, 1)) {
                                    l2 = 0;
                                    j2 = i1;
                                    k2 = k1;
                                }
                            }
                        }
                    }

                    for (int i3 = k1; i3 <= l1; ++i3) {
                        for (int j3 = i1; j3 <= j1; ++j3) {
                            if (j3 == j2 && i3 == k2) {
                                dst.set(j3, i3, 1048576 | l2 | i2 | k3);
                            } else {
                                dst.set(j3, i3, i2 | k3);
                            }
                        }
                    }

                    ++k3;
                }
            }
        }
    };

    
    class MansionTemplate : StructureComponentTemplate {
        std::string templateName;
        Rotation rotation;
        Mirror mirror;

    public: 
        MansionTemplate() : rotation({}), mirror({}) {}

        MansionTemplate(TemplateManager& manager, const std::string& name, Pos3D pos, Rotation rot)
        : StructureComponentTemplate(manager, name, pos, rot, Mirror::NONE) {
        }

        MansionTemplate(TemplateManager& manager, std::string name, const Pos3D pos, const Rotation rotation, const Mirror mirror)
            : StructureComponentTemplate(0) {
            this->templateName = std::move(name);
            this->m_templatePosition = pos;
            this->rotation = rotation;
            this->mirror = mirror;
            this->loadTemplate(manager);
        }

    private: 
        void loadTemplate(TemplateManager& manager) {
            Template m_template = manager.getTemplate((MinecraftServer) null, new ResourceLocation("mansion/" + this->templateName));
            PlacementSettings placementsettings = (new PlacementSettings()).setIgnoreEntities(true).setRotation(this->rotation).setMirror(this->mirror);
            this->setup(m_template, this->m_templatePosition, placementsettings);
        }

    protected:
        void writeStructureToNBT(NBTCompound& tagCompound) {
            this->writeStructureToNBT(tagCompound);
            tagCompound.setString("Template", this->templateName);
            tagCompound.setString("Rot", this->m_placeSettings.getRotation().name());
            tagCompound.setString("Mi", this->m_placeSettings.getMirror().name());
        }

        void readStructureFromNBT(NBTCompound& tagCompound, TemplateManager& manager) {
            this->readStructureFromNBT(tagCompound, manager);
            this->templateName = tagCompound.getString("Template");
            this->rotation = Rotation::valueOf(tagCompound.getString("Rot"));
            this->mirror = Mirror::valueOf(tagCompound.getString("Mi"));
            this->loadTemplate(manager);
        }

        void handleDataMarker(const std::string& marker, Pos3D pos, World world, RNG& random, BoundingBox& structureBB) {
            if (marker.starts_with("Chest")) {
                const Rotation rotation = this->m_placeSettings.getRotation();
                lce::BlockState iBlockState = lce::BlocksInit::CHEST.getDefaultState();

                if ("ChestWest".equals(marker)) {
                    iBlockState = iBlockState.withProperty(BlockChest.FACING, rotation.rotateFacing(EnumFacing::WEST));
                } else if ("ChestEast".equals(marker)) {
                    iBlockState = iBlockState.withProperty(BlockChest.FACING, rotation.rotateFacing(EnumFacing::EAST));
                } else if ("ChestSouth".equals(marker)) {
                    iBlockState = iBlockState.withProperty(BlockChest.FACING, rotation.rotateFacing(EnumFacing::SOUTH));
                } else if ("ChestNorth".equals(marker)) {
                    iBlockState = iBlockState.withProperty(BlockChest.FACING, rotation.rotateFacing(EnumFacing::NORTH));
                }

                this->func_191080_a(world, structureBB, random, pos, LootTableList.field_191192_o, iBlockState);
            } else if ("Mage" == marker) {
                // EntityEvoker entityEvoker = new EntityEvoker(world);
                // entityEvoker.enablePersistence();
                // entityEvoker.moveToBlockPosAndAngles(pos, 0.0F, 0.0F);
                // world.spawnEntityInWorld(entityEvoker);
                world.setBlockState(pos, lce::BlocksInit::AIR.getDefaultState(), 2);
            } else if ("Warrior" == marker) {
                // EntityVindicator entityVindicator = new EntityVindicator(world);
                // entityVindicator.enablePersistence();
                // entityVindicator.moveToBlockPosAndAngles(pos, 0.0F, 0.0F);
                // entityVindicator.onInitialSpawn(world.getDifficultyForLocation(new BlockPos(entityVindicator)), (IEntityLivingData) null);
                // world.spawnEntityInWorld(entityVindicator);
                world.setBlockState(pos, lce::BlocksInit::AIR.getDefaultState(), 2);
            }
        }
    };

    class PlacementData {
    public:
        PlacementData() {}

        Rotation rotation;
        Pos3D pos;
        std::string wallKey;
    };

    class Placer {
        TemplateManager& templates;
        RNG& random;
        int gridStartX;
        int gridStartZ;

    public: 
        Placer(TemplateManager& templatesIn, RNG& randomIn)
            : templates(templatesIn), random(randomIn) {}

        void placeMansion(Pos3D origin, Rotation rotation, std::list<MansionTemplate>& pieces, Grid& grid) {
            auto woodlandMansionPieces$placementData = PlacementData();
            woodlandMansionPieces$placementData.pos = origin;
            woodlandMansionPieces$placementData.rotation = rotation;
            woodlandMansionPieces$placementData.wallKey = "wall_flat";
            auto woodlandMansionPieces$placementData1 = PlacementData();
            this->addEntrance(pieces, woodlandMansionPieces$placementData);
            woodlandMansionPieces$placementData1.pos = woodlandMansionPieces$placementData.pos.up(8);
            woodlandMansionPieces$placementData1.rotation = woodlandMansionPieces$placementData.rotation;
            woodlandMansionPieces$placementData1.wallKey = "wall_window";

            if (!pieces.empty()) {
                ;
            }

            SimpleGrid woodlandMansionPieces$simpleGrid = grid.baseGrid;
            SimpleGrid woodlandMansionPieces$simpleGrid1 = grid.upperGrid;
            this->gridStartX = grid.originX + 1;
            this->gridStartZ = grid.originZ + 1;
            int i = grid.originX + 1;
            int j = grid.originZ;
            this->traceAndPlaceWalls(pieces, woodlandMansionPieces$placementData, woodlandMansionPieces$simpleGrid, EnumFacing::SOUTH, this->gridStartX, this->gridStartZ, i, j);
            this->traceAndPlaceWalls(pieces, woodlandMansionPieces$placementData1, woodlandMansionPieces$simpleGrid, EnumFacing::SOUTH, this->gridStartX, this->gridStartZ, i, j);
            auto woodlandMansionPieces$placementData2 = PlacementData();
            woodlandMansionPieces$placementData2.pos = woodlandMansionPieces$placementData.pos.up(19);
            woodlandMansionPieces$placementData2.rotation = woodlandMansionPieces$placementData.rotation;
            woodlandMansionPieces$placementData2.wallKey = "wall_window";
            bool flag = false;

            for (int k = 0; k < woodlandMansionPieces$simpleGrid1.height && !flag; ++k) {
                for (int l = woodlandMansionPieces$simpleGrid1.width - 1; l >= 0 && !flag; --l) {
                    if (Grid::isRoomOrHall(woodlandMansionPieces$simpleGrid1, l, k)) {
                        woodlandMansionPieces$placementData2.pos = woodlandMansionPieces$placementData2.pos.offset(rotation.rotateFacing(EnumFacing::SOUTH), 8 + (k - this->gridStartZ) * 8);
                        woodlandMansionPieces$placementData2.pos = woodlandMansionPieces$placementData2.pos.offset(rotation.rotateFacing(EnumFacing::EAST), (l - this->gridStartX) * 8);
                        this->placeWallPanel(pieces, woodlandMansionPieces$placementData2);
                        this->traceAndPlaceWalls(pieces, woodlandMansionPieces$placementData2, woodlandMansionPieces$simpleGrid1, EnumFacing::SOUTH, l, k, l, k);
                        flag = true;
                    }
                }
            }

            this->placeRoofs(pieces, origin.up(16), rotation, woodlandMansionPieces$simpleGrid, woodlandMansionPieces$simpleGrid1);
            this->placeRoofs(pieces, origin.up(27), rotation, woodlandMansionPieces$simpleGrid1, (SimpleGrid) null);

            if (!pieces.empty()) {
                ;
            }

            std::array<RoomCollection*, 3> aWoodlandMansionPieces$roomCollection = {};
            aWoodlandMansionPieces$roomCollection[0] = new FirstFloor();
            aWoodlandMansionPieces$roomCollection[1] = new SecondFloor();
            aWoodlandMansionPieces$roomCollection[2] = new ThirdFloor();

            for (int l2 = 0; l2 < 3; ++l2) {
                Pos3D blockPos = origin.up(8 * l2 + (l2 == 2 ? 3 : 0));
                SimpleGrid woodlandMansionPieces$simpleGrid2 = grid.floorGrid[l2];
                SimpleGrid woodlandMansionPieces$simpleGrid3 = l2 == 2 ? woodlandMansionPieces$simpleGrid1 : woodlandMansionPieces$simpleGrid;
                std::string s = l2 == 0 ? "carpet_south" : "carpet_south_2";
                std::string s1 = l2 == 0 ? "carpet_west" : "carpet_west_2";

                for (int i1 = 0; i1 < woodlandMansionPieces$simpleGrid3.height; ++i1) {
                    for (int j1 = 0; j1 < woodlandMansionPieces$simpleGrid3.width; ++j1) {
                        if (woodlandMansionPieces$simpleGrid3.get(j1, i1) == 1) {
                            Pos3D blockPos1 = blockPos.offset(rotation.rotateFacing(EnumFacing::SOUTH), 8 + (i1 - this->gridStartZ) * 8);
                            blockPos1 = blockPos1.offset(rotation.rotateFacing(EnumFacing::EAST), (j1 - this->gridStartX) * 8);
                            pieces.add(new MansionTemplate(this->templates, "corridor_floor", blockPos1, rotation));

                            if (woodlandMansionPieces$simpleGrid3.get(j1, i1 - 1) == 1 || (woodlandMansionPieces$simpleGrid2.get(j1, i1 - 1) & 8388608) == 8388608) {
                                pieces.add(new MansionTemplate(this->templates, "carpet_north", blockPos1.offset(rotation.rotateFacing(EnumFacing::EAST), 1).up(), rotation));
                            }

                            if (woodlandMansionPieces$simpleGrid3.get(j1 + 1, i1) == 1 || (woodlandMansionPieces$simpleGrid2.get(j1 + 1, i1) & 8388608) == 8388608) {
                                pieces.add(new MansionTemplate(this->templates, "carpet_east", blockPos1.offset(rotation.rotateFacing(EnumFacing::SOUTH), 1).offset(rotation.rotateFacing(EnumFacing::EAST), 5).up(), rotation));
                            }

                            if (woodlandMansionPieces$simpleGrid3.get(j1, i1 + 1) == 1 || (woodlandMansionPieces$simpleGrid2.get(j1, i1 + 1) & 8388608) == 8388608) {
                                pieces.add(new MansionTemplate(this->templates, s, blockPos1.offset(rotation.rotateFacing(EnumFacing::SOUTH), 5).offset(rotation.rotateFacing(EnumFacing::WEST), 1), rotation));
                            }

                            if (woodlandMansionPieces$simpleGrid3.get(j1 - 1, i1) == 1 || (woodlandMansionPieces$simpleGrid2.get(j1 - 1, i1) & 8388608) == 8388608) {
                                pieces.add(new MansionTemplate(this->templates, s1, blockPos1.offset(rotation.rotateFacing(EnumFacing::WEST), 1).offset(rotation.rotateFacing(EnumFacing::NORTH), 1), rotation));
                            }
                        }
                    }
                }

                std::string s2 = l2 == 0 ? "indoors_wall" : "indoors_wall_2";
                std::string s3 = l2 == 0 ? "indoors_door" : "indoors_door_2";
                std::list<EnumFacing> list = {};

                for (int k1 = 0; k1 < woodlandMansionPieces$simpleGrid3.height; ++k1) {
                    for (int l1 = 0; l1 < woodlandMansionPieces$simpleGrid3.width; ++l1) {
                        bool flag1 = l2 == 2 && woodlandMansionPieces$simpleGrid3.get(l1, k1) == 3;

                        if (woodlandMansionPieces$simpleGrid3.get(l1, k1) == 2 || flag1) {
                            int i2 = woodlandMansionPieces$simpleGrid2.get(l1, k1);
                            int j2 = i2 & 983040;
                            int k2 = i2 & 65535;
                            flag1 = flag1 && (i2 & 8388608) == 8388608;
                            list.clear();

                            if ((i2 & 2097152) == 2097152) {
                                for (EnumFacing enumFacing : EnumFacing::Plane.HORIZONTAL.facings()) {
                                    if (woodlandMansionPieces$simpleGrid3.get(l1 + enumFacing.getFrontOffsetX(), k1 + enumFacing.getFrontOffsetZ()) == 1) {
                                        list.add(enumFacing);
                                    }
                                }
                            }

                            EnumFacing enumFacing1;

                            if (!list.empty()) {
                                enumFacing1 = list.get(this->random.nextInt(list.size()));
                            } else if ((i2 & 1048576) == 1048576) {
                                enumFacing1 = EnumFacing::UP;
                            }

                            Pos3D blockPos2 = blockPos.offset(rotation.rotateFacing(EnumFacing::SOUTH), 8 + (k1 - this->gridStartZ) * 8);
                            blockPos2 = blockPos2.offset(rotation.rotateFacing(EnumFacing::EAST), -1 + (l1 - this->gridStartX) * 8);

                            if (Grid::isRoomOrHall(woodlandMansionPieces$simpleGrid3, l1 - 1, k1) && !grid.hasRoomId(woodlandMansionPieces$simpleGrid3, l1 - 1, k1, l2, k2)) {
                                pieces.add(new MansionTemplate(this->templates, enumFacing1 == EnumFacing::WEST ? s3 : s2, blockPos2, rotation));
                            }

                            if (woodlandMansionPieces$simpleGrid3.get(l1 + 1, k1) == 1 && !flag1) {
                                Pos3D blockPos3 = blockPos2.offset(rotation.rotateFacing(EnumFacing::EAST), 8);
                                pieces.add(new MansionTemplate(this->templates, enumFacing1 == EnumFacing::EAST ? s3 : s2, blockPos3, rotation));
                            }

                            if (Grid::isRoomOrHall(woodlandMansionPieces$simpleGrid3, l1, k1 + 1) && !grid.hasRoomId(woodlandMansionPieces$simpleGrid3, l1, k1 + 1, l2, k2)) {
                                Pos3D blockPos4 = blockPos2.offset(rotation.rotateFacing(EnumFacing::SOUTH), 7);
                                blockPos4 = blockPos4.offset(rotation.rotateFacing(EnumFacing::EAST), 7);
                                pieces.add(new MansionTemplate(this->templates, enumFacing1 == EnumFacing::SOUTH ? s3 : s2, blockPos4, rotation.add(Rotation::CLOCKWISE_90)));
                            }

                            if (woodlandMansionPieces$simpleGrid3.get(l1, k1 - 1) == 1 && !flag1) {
                                Pos3D blockPos5 = blockPos2.offset(rotation.rotateFacing(EnumFacing::NORTH), 1);
                                blockPos5 = blockPos5.offset(rotation.rotateFacing(EnumFacing::EAST), 7);
                                pieces.add(new MansionTemplate(this->templates, enumFacing1 == EnumFacing::NORTH ? s3 : s2, blockPos5, rotation.add(Rotation::CLOCKWISE_90)));
                            }

                            if (j2 == 65536) {
                                this->place1x1Room(pieces, blockPos2, rotation, enumFacing1, aWoodlandMansionPieces$roomCollection[l2]);
                            } else if (j2 == 131072 && enumFacing1 != null) {
                                EnumFacing enumFacing3 = grid.findNeighborWithId(woodlandMansionPieces$simpleGrid3, l1, k1, l2, k2);
                                bool flag2 = (i2 & 4194304) == 4194304;
                                this->place1x2Room(pieces, blockPos2, rotation, enumFacing3, enumFacing1, aWoodlandMansionPieces$roomCollection[l2], flag2);
                            } else if (j2 == 262144 && enumFacing1 != null && enumFacing1 != EnumFacing::UP) {
                                EnumFacing enumFacing2 = enumFacing1.rotateY();

                                if (!grid.hasRoomId(woodlandMansionPieces$simpleGrid3, l1 + enumFacing2.getFrontOffsetX(), k1 + enumFacing2.getFrontOffsetZ(), l2, k2)) {
                                    enumFacing2 = enumFacing2.getOpposite();
                                }

                                this->place2x2Room(pieces, blockPos2, rotation, enumFacing2, enumFacing1, aWoodlandMansionPieces$roomCollection[l2]);
                            } else if (j2 == 262144 && enumFacing1 == EnumFacing::UP) {
                                this->placeStairsRoom(pieces, blockPos2, rotation, aWoodlandMansionPieces$roomCollection[l2]);
                            }
                        }
                    }
                }
            }
        }

    private: 
        void traceAndPlaceWalls(std::list<MansionTemplate>& out, PlacementData& pd, SimpleGrid& grid, EnumFacing dir, const int startX, const int startZ, const int endX, const int endZ) {
            int i = startX;
            int j = startZ;
            const EnumFacing enumFacing = dir;

            while (true) {
                if (!Grid::isRoomOrHall(grid, i + dir.getFrontOffsetX(), j + dir.getFrontOffsetZ())) {
                    this->turnRightCorner(out, pd);
                    dir = dir.rotateY();

                    if (i != endX || j != endZ || enumFacing != dir) {
                        this->placeWallPanel(out, pd);
                    }
                } else if (Grid::isRoomOrHall(grid, i + dir.getFrontOffsetX(), j + dir.getFrontOffsetZ()) && Grid::isRoomOrHall(grid, i + dir.getFrontOffsetX() + dir.rotateYCCW().getFrontOffsetX(), j + dir.getFrontOffsetZ() + dir.rotateYCCW().getFrontOffsetZ())) {
                    turnLeftCorner(out, pd);
                    i += dir.getFrontOffsetX();
                    j += dir.getFrontOffsetZ();
                    dir = dir.rotateYCCW();
                } else {
                    i += dir.getFrontOffsetX();
                    j += dir.getFrontOffsetZ();

                    if (i != endX || j != endZ || enumFacing != dir) {
                        this->placeWallPanel(out, pd);
                    }
                }

                if (i == endX && j == endZ && enumFacing == dir) {
                    break;
                }
            }
        }

        void placeRoofs(std::list<MansionTemplate>& out, Pos3D roofBase, Rotation rot, SimpleGrid& grid, SimpleGrid& upper) {
            for (int i = 0; i < grid.height; ++i) {
                for (int j = 0; j < grid.width; ++j) {
                    Pos3D lvt_8_3_ = roofBase.offset(rot.rotateFacing(EnumFacing::SOUTH), 8 + (i - this->gridStartZ) * 8);
                    lvt_8_3_ = lvt_8_3_.offset(rot.rotateFacing(EnumFacing::EAST), (j - this->gridStartX) * 8);
                    bool flag = upper != null && Grid::isRoomOrHall(upper, j, i);

                    if (Grid::isRoomOrHall(grid, j, i) && !flag) {
                        out.add(new MansionTemplate(this->templates, "roof", lvt_8_3_.up(3), rot));

                        if (!Grid::isRoomOrHall(grid, j + 1, i)) {
                            Pos3D blockPos1 = lvt_8_3_.offset(rot.rotateFacing(EnumFacing::EAST), 6);
                            out.add(new MansionTemplate(this->templates, "roof_front", blockPos1, rot));
                        }

                        if (!Grid::isRoomOrHall(grid, j - 1, i)) {
                            Pos3D blockPos5 = lvt_8_3_.offset(rot.rotateFacing(EnumFacing::EAST), 0);
                            blockPos5 = blockPos5.offset(rot.rotateFacing(EnumFacing::SOUTH), 7);
                            out.add(new MansionTemplate(this->templates, "roof_front", blockPos5, rot.add(Rotation::CLOCKWISE_180)));
                        }

                        if (!Grid::isRoomOrHall(grid, j, i - 1)) {
                            Pos3D blockPos6 = lvt_8_3_.offset(rot.rotateFacing(EnumFacing::WEST), 1);
                            out.add(new MansionTemplate(this->templates, "roof_front", blockPos6, rot.add(Rotation::COUNTERCLOCKWISE_90)));
                        }

                        if (!Grid::isRoomOrHall(grid, j, i + 1)) {
                            Pos3D blockPos7 = lvt_8_3_.offset(rot.rotateFacing(EnumFacing::EAST), 6);
                            blockPos7 = blockPos7.offset(rot.rotateFacing(EnumFacing::SOUTH), 6);
                            out.add(new MansionTemplate(this->templates, "roof_front", blockPos7, rot.add(Rotation::CLOCKWISE_90)));
                        }
                    }
                }
            }

            if (upper != null) {
                for (int k = 0; k < grid.height; ++k) {
                    for (int i1 = 0; i1 < grid.width; ++i1) {
                        Pos3D blockPos3 = roofBase.offset(rot.rotateFacing(EnumFacing::SOUTH), 8 + (k - this->gridStartZ) * 8);
                        blockPos3 = blockPos3.offset(rot.rotateFacing(EnumFacing::EAST), (i1 - this->gridStartX) * 8);
                        bool flag1 = Grid::isRoomOrHall(upper, i1, k);

                        if (Grid::isRoomOrHall(grid, i1, k) && flag1) {
                            if (!Grid::isRoomOrHall(grid, i1 + 1, k)) {
                                Pos3D blockPos8 = blockPos3.offset(rot.rotateFacing(EnumFacing::EAST), 7);
                                out.add(new MansionTemplate(this->templates, "small_wall", blockPos8, rot));
                            }

                            if (!Grid::isRoomOrHall(grid, i1 - 1, k)) {
                                Pos3D blockPos9 = blockPos3.offset(rot.rotateFacing(EnumFacing::WEST), 1);
                                blockPos9 = blockPos9.offset(rot.rotateFacing(EnumFacing::SOUTH), 6);
                                out.add(new MansionTemplate(this->templates, "small_wall", blockPos9, rot.add(Rotation::CLOCKWISE_180)));
                            }

                            if (!Grid::isRoomOrHall(grid, i1, k - 1)) {
                                Pos3D blockPos10 = blockPos3.offset(rot.rotateFacing(EnumFacing::WEST), 0);
                                blockPos10 = blockPos10.offset(rot.rotateFacing(EnumFacing::NORTH), 1);
                                out.add(new MansionTemplate(this->templates, "small_wall", blockPos10, rot.add(Rotation::COUNTERCLOCKWISE_90)));
                            }

                            if (!Grid::isRoomOrHall(grid, i1, k + 1)) {
                                Pos3D blockPos11 = blockPos3.offset(rot.rotateFacing(EnumFacing::EAST), 6);
                                blockPos11 = blockPos11.offset(rot.rotateFacing(EnumFacing::SOUTH), 7);
                                out.add(new MansionTemplate(this->templates, "small_wall", blockPos11, rot.add(Rotation::CLOCKWISE_90)));
                            }

                            if (!Grid::isRoomOrHall(grid, i1 + 1, k)) {
                                if (!Grid::isRoomOrHall(grid, i1, k - 1)) {
                                    Pos3D blockPos12 = blockPos3.offset(rot.rotateFacing(EnumFacing::EAST), 7);
                                    blockPos12 = blockPos12.offset(rot.rotateFacing(EnumFacing::NORTH), 2);
                                    out.add(new MansionTemplate(this->templates, "small_wall_corner", blockPos12, rot));
                                }

                                if (!Grid::isRoomOrHall(grid, i1, k + 1)) {
                                    Pos3D blockPos13 = blockPos3.offset(rot.rotateFacing(EnumFacing::EAST), 8);
                                    blockPos13 = blockPos13.offset(rot.rotateFacing(EnumFacing::SOUTH), 7);
                                    out.add(new MansionTemplate(this->templates, "small_wall_corner", blockPos13, rot.add(Rotation::CLOCKWISE_90)));
                                }
                            }

                            if (!Grid::isRoomOrHall(grid, i1 - 1, k)) {
                                if (!Grid::isRoomOrHall(grid, i1, k - 1)) {
                                    Pos3D blockPos14 = blockPos3.offset(rot.rotateFacing(EnumFacing::WEST), 2);
                                    blockPos14 = blockPos14.offset(rot.rotateFacing(EnumFacing::NORTH), 1);
                                    out.add(new MansionTemplate(this->templates, "small_wall_corner", blockPos14, rot.add(Rotation::COUNTERCLOCKWISE_90)));
                                }

                                if (!Grid::isRoomOrHall(grid, i1, k + 1)) {
                                    Pos3D blockPos15 = blockPos3.offset(rot.rotateFacing(EnumFacing::WEST), 1);
                                    blockPos15 = blockPos15.offset(rot.rotateFacing(EnumFacing::SOUTH), 8);
                                    out.add(new MansionTemplate(this->templates, "small_wall_corner", blockPos15, rot.add(Rotation::CLOCKWISE_180)));
                                }
                            }
                        }
                    }
                }
            }

            for (int l = 0; l < grid.height; ++l) {
                for (int j1 = 0; j1 < grid.width; ++j1) {
                    Pos3D blockPos4 = roofBase.offset(rot.rotateFacing(EnumFacing::SOUTH), 8 + (l - this->gridStartZ) * 8);
                    blockPos4 = blockPos4.offset(rot.rotateFacing(EnumFacing::EAST), (j1 - this->gridStartX) * 8);
                    bool flag2 = upper != null && Grid::isRoomOrHall(upper, j1, l);

                    if (Grid::isRoomOrHall(grid, j1, l) && !flag2) {
                        if (!Grid::isRoomOrHall(grid, j1 + 1, l)) {
                            Pos3D blockPos16 = blockPos4.offset(rot.rotateFacing(EnumFacing::EAST), 6);

                            if (!Grid::isRoomOrHall(grid, j1, l + 1)) {
                                Pos3D blockPos2 = blockPos16.offset(rot.rotateFacing(EnumFacing::SOUTH), 6);
                                out.add(new MansionTemplate(this->templates, "roof_corner", blockPos2, rot));
                            } else if (Grid::isRoomOrHall(grid, j1 + 1, l + 1)) {
                                Pos3D blockPos18 = blockPos16.offset(rot.rotateFacing(EnumFacing::SOUTH), 5);
                                out.add(new MansionTemplate(this->templates, "roof_inner_corner", blockPos18, rot));
                            }

                            if (!Grid::isRoomOrHall(grid, j1, l - 1)) {
                                out.add(new MansionTemplate(this->templates, "roof_corner", blockPos16, rot.add(Rotation::COUNTERCLOCKWISE_90)));
                            } else if (Grid::isRoomOrHall(grid, j1 + 1, l - 1)) {
                                Pos3D blockPos19 = blockPos4.offset(rot.rotateFacing(EnumFacing::EAST), 9);
                                blockPos19 = blockPos19.offset(rot.rotateFacing(EnumFacing::NORTH), 2);
                                out.add(new MansionTemplate(this->templates, "roof_inner_corner", blockPos19, rot.add(Rotation::CLOCKWISE_90)));
                            }
                        }

                        if (!Grid::isRoomOrHall(grid, j1 - 1, l)) {
                            Pos3D blockPos17 = blockPos4.offset(rot.rotateFacing(EnumFacing::EAST), 0);
                            blockPos17 = blockPos17.offset(rot.rotateFacing(EnumFacing::SOUTH), 0);

                            if (!Grid::isRoomOrHall(grid, j1, l + 1)) {
                                Pos3D blockPos20 = blockPos17.offset(rot.rotateFacing(EnumFacing::SOUTH), 6);
                                out.add(new MansionTemplate(this->templates, "roof_corner", blockPos20, rot.add(Rotation::CLOCKWISE_90)));
                            } else if (Grid::isRoomOrHall(grid, j1 - 1, l + 1)) {
                                Pos3D blockPos21 = blockPos17.offset(rot.rotateFacing(EnumFacing::SOUTH), 8);
                                blockPos21 = blockPos21.offset(rot.rotateFacing(EnumFacing::WEST), 3);
                                out.add(new MansionTemplate(this->templates, "roof_inner_corner", blockPos21, rot.add(Rotation::COUNTERCLOCKWISE_90)));
                            }

                            if (!Grid::isRoomOrHall(grid, j1, l - 1)) {
                                out.add(new MansionTemplate(this->templates, "roof_corner", blockPos17, rot.add(Rotation::CLOCKWISE_180)));
                            } else if (Grid::isRoomOrHall(grid, j1 - 1, l - 1)) {
                                Pos3D blockPos22 = blockPos17.offset(rot.rotateFacing(EnumFacing::SOUTH), 1);
                                out.add(new MansionTemplate(this->templates, "roof_inner_corner", blockPos22, rot.add(Rotation::CLOCKWISE_180)));
                            }
                        }
                    }
                }
            }
        }

        void addEntrance(std::list<MansionTemplate>& out, PlacementData& pd) {
            const EnumFacing enumFacing = pd.rotation.rotateFacing(EnumFacing::WEST);
            out.add(new MansionTemplate(this->templates, "entrance", pd.pos.offset(enumFacing, 9), pd.rotation));
            pd.pos = pd.pos.offset(pd.rotation.rotateFacing(EnumFacing::SOUTH), 16);
        }

        void placeWallPanel(std::list<MansionTemplate>& out, PlacementData& pd) {
            out.add(new MansionTemplate(this->templates, pd.wallKey, pd.pos.offset(pd.rotation.rotateFacing(EnumFacing::EAST), 7), pd.rotation));
            pd.pos = pd.pos.offset(pd.rotation.rotateFacing(EnumFacing::SOUTH), 8);
        }

        void turnRightCorner(std::list<MansionTemplate>& out, PlacementData& pd) {
            pd.pos = pd.pos.offset(pd.rotation.rotateFacing(EnumFacing::SOUTH), -1);
            out.add(new MansionTemplate(this->templates, "wall_corner", pd.pos, pd.rotation));
            pd.pos = pd.pos.offset(pd.rotation.rotateFacing(EnumFacing::SOUTH), -7);
            pd.pos = pd.pos.offset(pd.rotation.rotateFacing(EnumFacing::WEST), -6);
            pd.rotation = pd.rotation.add(Rotation::CLOCKWISE_90);
        }

        static void turnLeftCorner(std::list<MansionTemplate>& out, PlacementData& pd) {
            pd.pos = pd.pos.offset(pd.rotation.rotateFacing(EnumFacing::SOUTH), 6);
            pd.pos = pd.pos.offset(pd.rotation.rotateFacing(EnumFacing::EAST), 8);
            pd.rotation = pd.rotation.add(Rotation::COUNTERCLOCKWISE_90);
        }

        void place1x1Room(std::list<MansionTemplate>& out, const Pos3D basePos, Rotation baseRot, const EnumFacing doorDir, RoomCollection* rooms) {
            Rotation rotation = Rotation::NONE;
            std::string s = rooms->get1x1Room(this->random);

            if (doorDir != EnumFacing::EAST) {
                if (doorDir == EnumFacing::NORTH) {
                    rotation = rotation.add(Rotation::COUNTERCLOCKWISE_90);
                } else if (doorDir == EnumFacing::WEST) {
                    rotation = rotation.add(Rotation::CLOCKWISE_180);
                } else if (doorDir == EnumFacing::SOUTH) {
                    rotation = rotation.add(Rotation::CLOCKWISE_90);
                } else {
                    s = rooms->get1x1Secret(this->random);
                }
            }

            Pos3D blockPos = Template::transformOrigin({1, 0, 0}, Mirror::NONE, rotation, 7, 7);
            rotation = rotation.add(baseRot);
            blockPos = blockPos.func_190942_a(baseRot);
            Pos3D blockPos1 = basePos.add(blockPos.getX(), 0, blockPos.getZ());
            out.add(new MansionTemplate(this->templates, s, blockPos1, rotation));
        }

        void place1x2Room(std::list<MansionTemplate>& out, const Pos3D basePos, Rotation baseRot, const EnumFacing hallDir, const EnumFacing doorDir, RoomCollection& rooms, const bool withStairs) {
            if (doorDir == EnumFacing::EAST && hallDir == EnumFacing::SOUTH) {
                Pos3D blockPos13 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 1);
                out.add(new MansionTemplate(this->templates, rooms.get1x2A(this->random, withStairs), blockPos13, baseRot));
            } else if (doorDir == EnumFacing::EAST && hallDir == EnumFacing::NORTH) {
                Pos3D blockPos12 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 1);
                blockPos12 = blockPos12.offset(baseRot.rotateFacing(EnumFacing::SOUTH), 6);
                out.add(new MansionTemplate(this->templates, rooms.get1x2A(this->random, withStairs), blockPos12, baseRot, Mirror::LEFT_RIGHT));
            } else if (doorDir == EnumFacing::WEST && hallDir == EnumFacing::NORTH) {
                Pos3D blockPos11 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 7);
                blockPos11 = blockPos11.offset(baseRot.rotateFacing(EnumFacing::SOUTH), 6);
                out.add(new MansionTemplate(this->templates, rooms.get1x2A(this->random, withStairs), blockPos11, baseRot.add(Rotation::CLOCKWISE_180)));
            } else if (doorDir == EnumFacing::WEST && hallDir == EnumFacing::SOUTH) {
                Pos3D blockPos10 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 7);
                out.add(new MansionTemplate(this->templates, rooms.get1x2A(this->random, withStairs), blockPos10, baseRot, Mirror::FRONT_BACK));
            } else if (doorDir == EnumFacing::SOUTH && hallDir == EnumFacing::EAST) {
                Pos3D blockPos9 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 1);
                out.add(new MansionTemplate(this->templates, rooms.get1x2A(this->random, withStairs), blockPos9, baseRot.add(Rotation::CLOCKWISE_90), Mirror::LEFT_RIGHT));
            } else if (doorDir == EnumFacing::SOUTH && hallDir == EnumFacing::WEST) {
                Pos3D blockPos8 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 7);
                out.add(new MansionTemplate(this->templates, rooms.get1x2A(this->random, withStairs), blockPos8, baseRot.add(Rotation::CLOCKWISE_90)));
            } else if (doorDir == EnumFacing::NORTH && hallDir == EnumFacing::WEST) {
                Pos3D blockPos7 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 7);
                blockPos7 = blockPos7.offset(baseRot.rotateFacing(EnumFacing::SOUTH), 6);
                out.add(new MansionTemplate(this->templates, rooms.get1x2A(this->random, withStairs), blockPos7, baseRot.add(Rotation::CLOCKWISE_90), Mirror::FRONT_BACK));
            } else if (doorDir == EnumFacing::NORTH && hallDir == EnumFacing::EAST) {
                Pos3D blockPos6 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 1);
                blockPos6 = blockPos6.offset(baseRot.rotateFacing(EnumFacing::SOUTH), 6);
                out.add(new MansionTemplate(this->templates, rooms.get1x2A(this->random, withStairs), blockPos6, baseRot.add(Rotation::COUNTERCLOCKWISE_90)));
            } else if (doorDir == EnumFacing::SOUTH && hallDir == EnumFacing::NORTH) {
                Pos3D blockPos5 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 1);
                blockPos5 = blockPos5.offset(baseRot.rotateFacing(EnumFacing::NORTH), 8);
                out.add(new MansionTemplate(this->templates, rooms.get1x2B(this->random, withStairs), blockPos5, baseRot));
            } else if (doorDir == EnumFacing::NORTH && hallDir == EnumFacing::SOUTH) {
                Pos3D blockPos4 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 7);
                blockPos4 = blockPos4.offset(baseRot.rotateFacing(EnumFacing::SOUTH), 14);
                out.add(new MansionTemplate(this->templates, rooms.get1x2B(this->random, withStairs), blockPos4, baseRot.add(Rotation::CLOCKWISE_180)));
            } else if (doorDir == EnumFacing::WEST && hallDir == EnumFacing::EAST) {
                Pos3D blockPos3 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 15);
                out.add(new MansionTemplate(this->templates, rooms.get1x2B(this->random, withStairs), blockPos3, baseRot.add(Rotation::CLOCKWISE_90)));
            } else if (doorDir == EnumFacing::EAST && hallDir == EnumFacing::WEST) {
                Pos3D blockPos2 = basePos.offset(baseRot.rotateFacing(EnumFacing::WEST), 7);
                blockPos2 = blockPos2.offset(baseRot.rotateFacing(EnumFacing::SOUTH), 6);
                out.add(new MansionTemplate(this->templates, rooms.get1x2B(this->random, withStairs), blockPos2, baseRot.add(Rotation::COUNTERCLOCKWISE_90)));
            } else if (doorDir == EnumFacing::UP && hallDir == EnumFacing::EAST) {
                Pos3D blockPos1 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 15);
                out.add(new MansionTemplate(this->templates, rooms.get1x2StairsEnd(this->random), blockPos1, baseRot.add(Rotation::CLOCKWISE_90)));
            } else if (doorDir == EnumFacing::UP && hallDir == EnumFacing::SOUTH) {
                Pos3D blockPos = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 1);
                blockPos = blockPos.offset(baseRot.rotateFacing(EnumFacing::NORTH), 0);
                out.add(new MansionTemplate(this->templates, rooms.get1x2StairsEnd(this->random), blockPos, baseRot));
            }
        }

        void place2x2Room(std::list<MansionTemplate>& out, const Pos3D basePos, const Rotation baseRot, const EnumFacing cornerDir, const EnumFacing doorDir, RoomCollection& rooms) {
            int i = 0;
            int j = 0;
            Rotation rotation = baseRot;
            Mirror mirror = Mirror::NONE;

            if (doorDir == EnumFacing::EAST && cornerDir == EnumFacing::SOUTH) {
                i = -7;
            } else if (doorDir == EnumFacing::EAST && cornerDir == EnumFacing::NORTH) {
                i = -7;
                j = 6;
                mirror = Mirror::LEFT_RIGHT;
            } else if (doorDir == EnumFacing::NORTH && cornerDir == EnumFacing::EAST) {
                i = 1;
                j = 14;
                rotation = baseRot.add(Rotation::COUNTERCLOCKWISE_90);
            } else if (doorDir == EnumFacing::NORTH && cornerDir == EnumFacing::WEST) {
                i = 7;
                j = 14;
                rotation = baseRot.add(Rotation::COUNTERCLOCKWISE_90);
                mirror = Mirror::LEFT_RIGHT;
            } else if (doorDir == EnumFacing::SOUTH && cornerDir == EnumFacing::WEST) {
                i = 7;
                j = -8;
                rotation = baseRot.add(Rotation::CLOCKWISE_90);
            } else if (doorDir == EnumFacing::SOUTH && cornerDir == EnumFacing::EAST) {
                i = 1;
                j = -8;
                rotation = baseRot.add(Rotation::CLOCKWISE_90);
                mirror = Mirror::LEFT_RIGHT;
            } else if (doorDir == EnumFacing::WEST && cornerDir == EnumFacing::NORTH) {
                i = 15;
                j = 6;
                rotation = baseRot.add(Rotation::CLOCKWISE_180);
            } else if (doorDir == EnumFacing::WEST && cornerDir == EnumFacing::SOUTH) {
                i = 15;
                mirror = Mirror::FRONT_BACK;
            }

            Pos3D blockPos = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), i);
            blockPos = blockPos.offset(baseRot.rotateFacing(EnumFacing::SOUTH), j);
            out.add(new MansionTemplate(this->templates, rooms.get2x2Room(this->random), blockPos, rotation, mirror));
        }

        void placeStairsRoom(std::list<MansionTemplate>& out, Pos3D basePos, Rotation baseRot, RoomCollection& rooms) {
            Pos3D blockPos = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 1);
            out.add(new MansionTemplate(this->templates, rooms.get2x2Secret(this->random), blockPos, baseRot, Mirror::NONE));
        }
    };

    class SecondFloor : public RoomCollection {
    public:
        SecondFloor() = default;

        std::string get1x1Room(RNG& rand) override {
            const char value = getIntChar(rand.nextInt(4) + 1);
            return "1x1_b" + std::string(1, value);
        }

        std::string get1x1Secret(RNG& rand) override {
            const char value = getIntChar(rand.nextInt(4) + 1);
            return "1x1_as" + std::string(1, value);
        }

        std::string get1x2A(RNG& rand, const bool withStairs) override {
            if (withStairs) {
                return "1x2_c_stairs";
            }
            const char value = getIntChar(rand.nextInt(4) + 1);
            return "1x2_c" + std::string(1, value);
        }

        std::string get1x2B(RNG& rand, const bool withStairs) override {
            if (withStairs) {
                return "1x2_d_stairs";
            }
            const char value = getIntChar(rand.nextInt(5) + 1);
            return "1x2_d" + std::string(1, value);
        }

        std::string get1x2StairsEnd(RNG& rand) override {
            const char value = getIntChar(rand.nextInt(1) + 1);
            return "1x2_se" + std::string(1, value);
        }

        std::string get2x2Room(RNG& rand) override {
            const char value = getIntChar(rand.nextInt(5) + 1);
            return "2x2_b" + std::string(1, value);
        }

        std::string get2x2Secret(RNG& rand) override {
            return "2x2_s1";
        }
    };

    class ThirdFloor final : public SecondFloor {
    public:
        ThirdFloor() = default;
    };
};















