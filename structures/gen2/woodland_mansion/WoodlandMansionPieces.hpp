#pragma once

#include <list>
#include <string>
#include <utility>
#include <unordered_map>
#include <array>

#include "common/RotationAndMirror.hpp"
#include "common/rng.hpp"

#include "Seedra/structures/gen2/ResourceLocation.hpp"
#include "Seedra/structures/gen2/StructureComponentTemplate.hpp"
#include "Seedra/structures/gen2/Template.hpp"
#include "Seedra/structures/gen2/TemplateManager.hpp"


class MU WoodlandMansionPieces {

public:
    class MansionTemplate;


    enum Room : int {
        EMPTY = 0,
        CORRIDOR = 1,
        JUNCTION = 2,
        ENTRANCE = 3,
        SPECIAL = 4,
        WALL = 5
    };

    enum RoomSize : int {
        SIZE_1X1 = 65536,    // 0x10000
        SIZE_1X2 = 131072,   // 0x20000
        SIZE_2X2 = 262144    // 0x40000
    };

    enum RoomFlags : int {
        DOOR_POSITION = 1048576,        // 0x100000 - Marks which cell in multi-cell room has the door
        CONNECTS_TO_CORRIDOR = 2097152, // 0x200000 - Room has adjacent corridor access
        HAS_STAIRCASE = 4194304,        // 0x400000 - 1x2 room contains stairs to next floor
        STAIRWELL_SHAFT = 8388608       // 0x800000 - Vertical stairwell connecting floors
    };


    enum RoomMask : int {
        ROOM_SIZE_MASK = 983040, // 0xF0000 - Extract room size bits
        ROOM_ID_MASK = 65535     // 0xFFFF - Extract unique room ID
    };



    // MU static void registerPieces() {
    //     // MapGenStructureIO.registerStructureComponent(WoodlandMansionPieces::MansionTemplate*, "WMP");
    // }
    
    MU static void generateAt(TemplateManager& templateManager, const Pos3D origin, const Rotation rotation,
                           std::list<WoodlandMansionPieces::MansionTemplate>& pieces, RNG& random) {
        Grid woodlandMansionPieces$grid = Grid(random);
        Placer woodlandMansionPieces$placer = Placer(templateManager, random);
        woodlandMansionPieces$placer.placeMansion(origin, rotation, pieces, woodlandMansionPieces$grid);
    }


    class SimpleGrid {
    public:
        std::vector<std::vector<int>> cells;
        mutable int width{}; // should be const but Java sucks
        mutable int height{}; // should be const but Java sucks
        mutable int defaultValue{}; // should be const but Java sucks

        SimpleGrid() = default;

        SimpleGrid(const int widthIn, const int heightIn, const int defaultValue)
            : width(widthIn), height(heightIn), defaultValue(defaultValue) {
            cells.reserve(widthIn);
            for (int i = 0; i < widthIn; i++) {
                this->cells.emplace_back(heightIn, 0);
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

        int get(int x, int z) const {
            return x >= 0 && x < this->width && z >= 0 && z < this->height ? this->cells[x][z] : this->defaultValue;
        }

        void printGrid(const std::string& name) const {
            using std::cout;
            using std::endl;

            // ANSI escape codes
            const std::string reset = "\033[0m";

            // 8-bit color palette (change or trim as you like)
            static const std::array<const char*, 16> ROOM_COLORS = {
                    "\033[38;5;196m", // red
                    "\033[38;5;202m", // orange
                    "\033[38;5;220m", // yellow
                    "\033[38;5;82m",  // green
                    "\033[38;5;45m",  // cyan
                    "\033[38;5;27m",  // blue
                    "\033[38;5;93m",  // magenta
                    "\033[38;5;201m", // pink
                    "\033[38;5;130m", // brown
                    "\033[38;5;35m",  // teal
                    "\033[38;5;141m", // purple
                    "\033[38;5;190m", // yellow-green
                    "\033[38;5;208m", // gold
                    "\033[38;5;160m", // dark red
                    "\033[38;5;33m",  // deep blue
                    "\033[38;5;99m"   // violet
            };

            // First pass: map room IDs (lower 16 bits) to a color index
            std::unordered_map<int, int> idToColorIndex;
            int nextColor = 0;

            for (int z = 0; z < height; ++z) {
                for (int x = 0; x < width; ++x) {
                    int v = get(x, z);
                    if (v & ROOM_SIZE_MASK) {
                        int id = v & ROOM_ID_MASK;
                        if (!idToColorIndex.count(id)) {
                            idToColorIndex[id] = nextColor % static_cast<int>(ROOM_COLORS.size());
                            ++nextColor;
                        }
                    }
                }
            }

            cout << "\n=== GRID: " << name << " (" << width << "x" << height << ") ===" << endl;

            // Header row
            cout << "    ";
            for (int x = 0; x < width; ++x) {
                cout << std::setw(2) << x << ' ';
            }
            cout << endl;

            for (int z = 0; z < height; ++z) {
                cout << std::setw(2) << z << ": ";

                for (int x = 0; x < width; ++x) {
                    int v = get(x, z);

                    char glyph = '.';           // default empty
                    std::string color = "";     // default no color

                    bool isRoom   = (v & ROOM_SIZE_MASK) != 0;
                    bool hasDoor  = (v & DOOR_POSITION) != 0;
                    bool hasStair = (v & STAIRWELL_SHAFT) != 0;

                    if (!isRoom) {
                        // Base layout cells (no room bits set)
                        switch (v) {
                            case EMPTY:
                                glyph = '.';
                                color = "\033[38;5;239m"; // dim gray
                                break;
                            case CORRIDOR:
                                glyph = '#';
                                color = "\033[38;5;250m"; // light gray
                                break;
                            case JUNCTION:
                                glyph = '+';
                                color = "\033[38;5;15m";  // bright white
                                break;
                            case ENTRANCE:
                                glyph = 'E';
                                color = "\033[38;5;82m";  // green
                                break;
                            case SPECIAL:
                                glyph = '?';
                                color = "\033[38;5;213m"; // pink-ish
                                break;
                            case WALL:
                                glyph = 'X';
                                color = "\033[38;5;244m"; // wall gray
                                break;
                            default:
                                glyph = '.';
                                color = "\033[38;5;239m";
                                break;
                        }
                    } else {
                        // Room tiles
                        int sizeBits = v & ROOM_SIZE_MASK;
                        int id       = v & ROOM_ID_MASK;

                        // choose glyph by size, overridden by flags
                        if (sizeBits == SIZE_1X1)      glyph = 'R';
                        else if (sizeBits == SIZE_1X2) glyph = 'L';
                        else if (sizeBits == SIZE_2X2) glyph = 'B';
                        else                            glyph = 'R';

                        // Highlight door / stair visually:
                        if (hasStair) {
                            glyph = '^'; // vertical shaft marker
                        } else if (hasDoor) {
                            glyph = 'D'; // door cell within the room
                        }

                        auto it = idToColorIndex.find(id);
                        if (it != idToColorIndex.end()) {
                            color = ROOM_COLORS[it->second];
                        } else {
                            color = "\033[38;5;15m"; // fallback bright white
                        }
                    }

                    cout << color << glyph << ' ' << reset;
                }

                cout << endl;
            }

            // Optional small legend
            cout << "\nLegend:\n"
                 << "  " << "\033[38;5;239m.\033[0m" << " = empty\n"
                 << "  " << "\033[38;5;250m#\033[0m" << " = corridor\n"
                 << "  " << "\033[38;5;15m+\033[0m"  << " = junction\n"
                 << "  " << "\033[38;5;82mE\033[0m"  << " = entrance\n"
                 << "  " << "\033[38;5;244mX\033[0m" << " = wall\n"
                 << "  colored " << "R/L/B = rooms (1x1 / 1x2 / 2x2) by ID\n"
                 << "  colored " << "D = door cell in multi-tile room\n"
                 << "  colored " << "^\033[0m = stairwell shaft\n"
                 << endl;
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
        virtual std::string get1x1Room(RNG& rand) = 0;
        virtual std::string get1x1Secret(RNG& rand) = 0;
        virtual std::string get1x2A(RNG& rand, bool withStairs) = 0;
        virtual std::string get1x2B(RNG& rand, bool withStairs) = 0;
        virtual std::string get1x2StairsEnd(RNG& rand) = 0;
        virtual std::string get2x2Room(RNG& rand) = 0;
        virtual std::string get2x2Secret(RNG& rand) = 0;

    protected:
        static char getIntChar(const int value) {
            static std::array<char, 10> CHARACTERS = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
            return CHARACTERS[value];
        }
    };

    

    class FirstFloor final : public RoomCollection {

    public:
        FirstFloor() = default;

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

        static constexpr int WIDTH = 11;
        static constexpr int HEIGHT = 11;

        explicit Grid(RNG& random) : random(random),
                            originX(7),
                            originZ(4),
                            baseGrid(11, 11, 5),
                            upperGrid(11, 11, 5),
                            floorGrid({
                                {11, 11, 5},
                                {11, 11, 5},
                                {11, 11, 5}
                            }) {
            std::cout << "\n=== GRID INITIALIZATION ===" << std::endl;
            std::cout << "Grid origin: (" << originX << ", " << originZ << ")" << std::endl;


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

            std::cout << "Initial base grid setup:" << std::endl;
            this->baseGrid.printGrid("BASE INITIAL");

            this->carvePath(this->baseGrid, this->originX, this->originZ - 2, EnumFacing::WEST, 6);
            this->carvePath(this->baseGrid, this->originX, this->originZ + 3, EnumFacing::WEST, 6);
            this->carvePath(this->baseGrid, this->originX - 2, this->originZ - 1, EnumFacing::WEST, 3);
            this->carvePath(this->baseGrid, this->originX - 2, this->originZ + 2, EnumFacing::WEST, 3);

            std::cout << "\nAfter carving paths:" << std::endl;
            this->baseGrid.printGrid("BASE CARVED");

            int iterations = 0;
            while (expandJunctions(this->baseGrid)) {
                iterations++;
            }
            std::cout << "Expanded junctions in " << iterations << " iterations" << std::endl;
            this->baseGrid.printGrid("BASE EXPANDED");

            this->copyLayout(this->baseGrid, this->floorGrid[0]);
            this->copyLayout(this->baseGrid, this->floorGrid[1]);

            std::cout << "\nFloor grids after copy:" << std::endl;
            this->floorGrid[0].printGrid("FLOOR 0");
            this->floorGrid[1].printGrid("FLOOR 1");

            this->floorGrid[0].fill(this->originX + 1, this->originZ, this->originX + 1, this->originZ + 1, 8388608);
            this->floorGrid[1].fill(this->originX + 1, this->originZ, this->originX + 1, this->originZ + 1, 8388608);

            this->planUpperFloor();
            this->copyLayout(this->upperGrid, this->floorGrid[2]);

            std::cout << "\nFinal floor grids:" << std::endl;
            this->floorGrid[0].printGrid("FLOOR 0 FINAL");
            this->floorGrid[1].printGrid("FLOOR 1 FINAL");
            this->floorGrid[2].printGrid("FLOOR 2 FINAL");
            this->upperGrid.printGrid("UPPER GRID");
        }
        
        static bool isRoomOrHall(SimpleGrid& grid, const int x, const int z) {
            int i = grid.get(x, z);
            return i == 1 || i == 2 || i == 3 || i == 4;
        }

        bool hasRoomId(SimpleGrid& _g, const int x, const int z, const int floorIdx, const int id) {
            return (this->floorGrid[floorIdx].get(x, z) & 65535) == id;
        }

        std::optional<EnumFacing> findNeighborWithId(SimpleGrid& g, int x, int z, const int floorIdx, const int id) {
            for (EnumFacing enumFacing: FACING_HORIZONTAL) {
                if (this->hasRoomId(g, x + getFrontOffsetX(enumFacing), z + getFrontOffsetZ(enumFacing), floorIdx, id)) {
                    return enumFacing;
                }
            }

            return std::nullopt;
        }

    private: 
        void carvePath(SimpleGrid& g, int x, int z, EnumFacing dir, const int depth) {
            if (depth > 0) {
                g.set(x, z, 1);
                g.setIfEquals(x + getFrontOffsetX(dir), z + getFrontOffsetZ(dir), 0, 1);

                for (int i = 0; i < 8; ++i) {
                    EnumFacing enumFacing = getHorizontal(this->random.nextInt(4));

                    if (enumFacing != getOppositeFacing(dir) && (enumFacing != EnumFacing::EAST || !this->random.nextBoolean())) {
                        int j = x + getFrontOffsetX(dir);
                        int k = z + getFrontOffsetZ(dir);

                        if (g.get(j + getFrontOffsetX(enumFacing), k + getFrontOffsetZ(enumFacing)) == 0 && g.get(j + getFrontOffsetX(enumFacing) * 2, k + getFrontOffsetZ(enumFacing) * 2) == 0) {
                            this->carvePath(g, x + getFrontOffsetX(dir) + getFrontOffsetX(enumFacing), z + getFrontOffsetZ(dir) + getFrontOffsetZ(enumFacing), enumFacing, depth - 1);
                            break;
                        }
                    }
                }

                EnumFacing enumFacing1 = rotateY(dir);
                EnumFacing enumFacing2 = rotateYCCW(dir);
                g.setIfEquals(x + getFrontOffsetX(enumFacing1), z + getFrontOffsetZ(enumFacing1), 0, 2);
                g.setIfEquals(x + getFrontOffsetX(enumFacing2), z + getFrontOffsetZ(enumFacing2), 0, 2);
                g.setIfEquals(x + getFrontOffsetX(dir) + getFrontOffsetX(enumFacing1), z + getFrontOffsetZ(dir) + getFrontOffsetZ(enumFacing1), 0, 2);
                g.setIfEquals(x + getFrontOffsetX(dir) + getFrontOffsetX(enumFacing2), z + getFrontOffsetZ(dir) + getFrontOffsetZ(enumFacing2), 0, 2);
                g.setIfEquals(x + getFrontOffsetX(dir) * 2, z + getFrontOffsetZ(dir) * 2, 0, 2);
                g.setIfEquals(x + getFrontOffsetX(enumFacing1) * 2, z + getFrontOffsetZ(enumFacing1) * 2, 0, 2);
                g.setIfEquals(x + getFrontOffsetX(enumFacing2) * 2, z + getFrontOffsetZ(enumFacing2) * 2, 0, 2);
            }
        }

        static bool expandJunctions(SimpleGrid& g) {
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
            SimpleGrid& woodlandMansionPieces$simpleGrid = this->floorGrid[1];

            for (int i = 0; i < this->upperGrid.height; ++i) {
                for (int j = 0; j < this->upperGrid.width; ++j) {
                    const int k = woodlandMansionPieces$simpleGrid.get(j, i);
                    const int l = k & 983040;

                    if (l == 131072 && (k & 2097152) == 2097152) {
                        list.emplace_back(j, i);
                    }
                }
            }

            if (list.empty()) {
                this->upperGrid.fill(0, 0, this->upperGrid.width, this->upperGrid.height, 5);
            } else {
                // std::pair<int, int> tuple = list.get(this->random.nextInt(list.size()));
                auto it = list.begin();
                std::advance(it, this->random.nextInt(list.size()));
                std::pair<int, int> tuple = *it;



                const int l1 = woodlandMansionPieces$simpleGrid.get(tuple.first, tuple.second);
                woodlandMansionPieces$simpleGrid.set(tuple.first, tuple.second, l1 | 4194304);
                auto enumFacing1 =
                        this->findNeighborWithId(this->baseGrid, tuple.first, tuple.second, 1, (l1 & ROOM_ID_MASK));

                assert(enumFacing1 && "planUpperFloor expects a valid neighbor (Java would NPE here)");

                int i2 = tuple.first + getFrontOffsetX(*enumFacing1);
                int i1 = tuple.second + getFrontOffsetZ(*enumFacing1);

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

                for (EnumFacing enumFacing: FACING_HORIZONTAL) {
                    if (this->upperGrid.get(i2 + getFrontOffsetX(enumFacing), i1 + getFrontOffsetZ(enumFacing)) == 0) {
                        list1.push_back(enumFacing);
                    }
                }

                if (list1.empty()) {
                    this->upperGrid.fill(0, 0, this->upperGrid.width, this->upperGrid.height, 5);
                    woodlandMansionPieces$simpleGrid.set(tuple.first, tuple.second, l1);
                } else {
                    // const EnumFacing enumFacing2 = list1.at(this->random.nextInt(list1.size()));
                    auto it1 = list1.begin();
                    std::advance(it1, this->random.nextInt(list1.size()));
                    EnumFacing enumFacing2 = *it1;

                    this->carvePath(this->upperGrid, i2 + getFrontOffsetX(enumFacing2), i1 + getFrontOffsetZ(enumFacing2), enumFacing2, 4);

                    while (expandJunctions(this->upperGrid)) {
                        // ;
                    }
                }
            }
        }

        template<typename T>
        void Collections_shuffle(std::vector<T>& vec, RNG& rnd) {
            std::size_t size = vec.size();
            for (std::size_t i = size; i > 1; --i) {
                auto j = static_cast<std::size_t>(rnd.nextInt(static_cast<int>(i)));
                std::swap(vec[i - 1], vec[j]);
            }
        }

        void copyLayout(SimpleGrid& src, SimpleGrid& dst) {
            std::list<std::pair<int, int>> list = {};

            for (int i = 0; i < src.height; ++i) {
                for (int j = 0; j < src.width; ++j) {
                    if (src.get(j, i) == 2) {
                        list.emplace_back(j, i);
                    }
                }
            }

            std::vector<std::pair<int, int>> vec(list.begin(), list.end());

            Collections_shuffle(vec, this->random);
            int k3 = 10;

            for (auto& [fst, snd] : vec) {
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

    
    class MansionTemplate : public StructureComponentTemplate {
        std::string templateName;
        Rotation rotation = Rotation::NONE;
        Mirror mirror = Mirror::NONE;

    public: 
        MansionTemplate() : rotation({}), mirror({}) {}

        MansionTemplate(TemplateManager& manager, const std::string& name, Pos3D pos, Rotation rot)
        : MansionTemplate(manager, name, pos, rot, Mirror::NONE) {
        }

        MansionTemplate(TemplateManager& manager, std::string name, const Pos3D pos, const Rotation rotation, const Mirror mirror)
            : StructureComponentTemplate(static_cast<const PieceType>(0)) {
            this->templateName = std::move(name);
            this->m_templatePosition = pos;
            this->rotation = rotation;
            this->mirror = mirror;

            std::cout << "Creating piece: " << templateName
                      << " at (" << pos.getX() << "," << pos.getY() << "," << pos.getZ() << ")"
                      << " rot=" << static_cast<int>(rotation.type)
                      << " mir=" << static_cast<int>(mirror.type) << std::endl;

            this->loadTemplate(manager);

            std::cout << "  -> Bounding box: ("
                      << m_minX << "," << m_minY << "," << m_minZ
                      << ") to ("
                      << m_maxX << "," << m_maxY << "," << m_maxZ
                      << ")" << std::endl;
        }

        std::string getName() { return templateName; }

    private: 
        void loadTemplate(TemplateManager& manager) {
            Template* m_template = manager.getTemplate(ResourceLocation("mansion/" + this->templateName));
            PlacementSettings placementSettings = PlacementSettings().setIgnoreEntities(true).setRotation(this->rotation).setMirror(this->mirror);
            this->setup(m_template, this->m_templatePosition, placementSettings);
        }

    protected:
        /*
        void writeStructureToNBT(NBTCompound& tagCompound) {
            this->writeStructureToNBT(tagCompound);
            tagCompound.setString("Template", this->templateName);
            tagCompound.setString("Rot", this->m_placeSettings.getRotation().name());
            tagCompound.setString("Mi", this->m_placeSettings.getMirror().name());
        }
         */

        /*
        void readStructureFromNBT(NBTCompound& tagCompound, TemplateManager& manager) {
            this->readStructureFromNBT(tagCompound, manager);
            this->templateName = tagCompound.getString("Template");
            this->rotation = Rotation::valueOf(tagCompound.getString("Rot"));
            this->mirror = Mirror::valueOf(tagCompound.getString("Mi"));
            this->loadTemplate(manager);
        }
         */

        MU void handleDataMarker(const std::string& marker, Pos3D pos, World& world, RNG& random, BoundingBox& structureBB) {
            // if (marker.starts_with("Chest")) {
            //     const Rotation rotation = this->m_placeSettings.getRotation();
            //     lce::BlockState iBlockState = lce::BlocksInit::CHEST.getDefaultState();
            //     if ("ChestWest" == marker) {
            //         iBlockState = iBlockState.withProperty(BlockChest.FACING, rotation.rotateFacing(EnumFacing::WEST));
            //     } else if ("ChestEast" == marker) {
            //         iBlockState = iBlockState.withProperty(BlockChest.FACING, rotation.rotateFacing(EnumFacing::EAST));
            //     } else if ("ChestSouth" == marker) {
            //         iBlockState = iBlockState.withProperty(BlockChest.FACING, rotation.rotateFacing(EnumFacing::SOUTH));
            //     } else if ("ChestNorth" == marker) {
            //         iBlockState = iBlockState.withProperty(BlockChest.FACING, rotation.rotateFacing(EnumFacing::NORTH));
            //     }
            //     this->func_191080_a(world, structureBB, random, pos, LootTableList.field_191192_o, iBlockState);
            // } else if ("Mage" == marker) {
            //     // EntityEvoker entityEvoker = new EntityEvoker(world);
            //     // entityEvoker.enablePersistence();
            //     // entityEvoker.moveToBlockPosAndAngles(pos, 0.0F, 0.0F);
            //     // world.spawnEntityInWorld(entityEvoker);
            //     world.setBlock(pos, lce::BlocksInit::AIR.getDefaultState()/*, 2*/);
            // } else if ("Warrior" == marker) {
            //     // EntityVindicator entityVindicator = new EntityVindicator(world);
            //     // entityVindicator.enablePersistence();
            //     // entityVindicator.moveToBlockPosAndAngles(pos, 0.0F, 0.0F);
            //     // entityVindicator.onInitialSpawn(world.getDifficultyForLocation(new BlockPos(entityVindicator)), (IEntityLivingData) null);
            //     // world.spawnEntityInWorld(entityVindicator);
            //     world.setBlock(pos, lce::BlocksInit::AIR.getDefaultState()/*, 2*/);
            // }
        }
    };

    class PlacementData {
    public:
        PlacementData() = default;

        Rotation rotation = Rotation::NONE;
        Pos3D pos;
        std::string wallKey;
    };

    class Placer {
        TemplateManager& templates;
        RNG& random;
        int gridStartX{};
        int gridStartZ{};

    public: 
        Placer(TemplateManager& templatesIn, RNG& randomIn)
            : templates(templatesIn), random(randomIn) {}

        void placeMansion(Pos3D origin, Rotation rotation, std::list<MansionTemplate>& pieces, Grid& grid) {
            auto wMP$pD = PlacementData();
            wMP$pD.pos = origin;
            wMP$pD.rotation = rotation;
            wMP$pD.wallKey = "wall_flat";
            auto wMP$pD1 = PlacementData();
            this->addEntrance(pieces, wMP$pD);
            wMP$pD1.pos = wMP$pD.pos.up(8);
            wMP$pD1.rotation = wMP$pD.rotation;
            wMP$pD1.wallKey = "wall_window";

            if (!pieces.empty()) {
                // ;
            }

            SimpleGrid& woodlandMansionPieces$simpleGrid = grid.baseGrid;
            SimpleGrid& woodlandMansionPieces$simpleGrid1 = grid.upperGrid;
            this->gridStartX = grid.originX + 1;
            this->gridStartZ = grid.originZ + 1;
            int i = grid.originX + 1;
            int j = grid.originZ;
            this->traceAndPlaceWalls(pieces, wMP$pD, woodlandMansionPieces$simpleGrid, EnumFacing::SOUTH, this->gridStartX, this->gridStartZ, i, j);
            this->traceAndPlaceWalls(pieces, wMP$pD1, woodlandMansionPieces$simpleGrid, EnumFacing::SOUTH, this->gridStartX, this->gridStartZ, i, j);
            auto wMP$pD2 = PlacementData();
            wMP$pD2.pos = wMP$pD.pos.up(19);
            wMP$pD2.rotation = wMP$pD.rotation;
            wMP$pD2.wallKey = "wall_window";
            bool flag = false;

            for (int k = 0; k < woodlandMansionPieces$simpleGrid1.height && !flag; ++k) {
                for (int l = woodlandMansionPieces$simpleGrid1.width - 1; l >= 0 && !flag; --l) {
                    if (Grid::isRoomOrHall(woodlandMansionPieces$simpleGrid1, l, k)) {
                        wMP$pD2.pos = wMP$pD2.pos.offset(rotation.rotateFacing(EnumFacing::SOUTH), 8 + (k - this->gridStartZ) * 8);
                        wMP$pD2.pos =
                                wMP$pD2.pos.offset(rotation.rotateFacing(EnumFacing::EAST), (l - this->gridStartX) * 8);
                        this->placeWallPanel(pieces, wMP$pD2);
                        this->traceAndPlaceWalls(pieces, wMP$pD2, woodlandMansionPieces$simpleGrid1, EnumFacing::SOUTH, l, k, l, k);
                        flag = true;
                    }
                }
            }

            this->placeRoofs(pieces, origin.up(16), rotation, woodlandMansionPieces$simpleGrid, &woodlandMansionPieces$simpleGrid1);
            this->placeRoofs(pieces, origin.up(27), rotation, woodlandMansionPieces$simpleGrid1, nullptr);

            if (!pieces.empty()) {
                // ;
            }

            FirstFloor f0;
            SecondFloor f1;
            ThirdFloor f2;
            std::array<RoomCollection*,3> aWoodlandMansionPieces$roomCollection = { &f0, &f1, &f2 };

            for (int l2 = 0; l2 < 3; ++l2) {
                Pos3D blockPos = origin.up(8 * l2 + (l2 == 2 ? 3 : 0));
                SimpleGrid& woodlandMansionPieces$simpleGrid2 = grid.floorGrid[l2];
                SimpleGrid& woodlandMansionPieces$simpleGrid3 = l2 == 2 ? woodlandMansionPieces$simpleGrid1 : woodlandMansionPieces$simpleGrid;
                std::string s = l2 == 0 ? "carpet_south" : "carpet_south_2";
                std::string s1 = l2 == 0 ? "carpet_west" : "carpet_west_2";

                for (int i1 = 0; i1 < woodlandMansionPieces$simpleGrid3.height; ++i1) {
                    for (int j1 = 0; j1 < woodlandMansionPieces$simpleGrid3.width; ++j1) {
                        if (woodlandMansionPieces$simpleGrid3.get(j1, i1) == 1) {
                            Pos3D blockPos1 = blockPos.offset(rotation.rotateFacing(EnumFacing::SOUTH), 8 + (i1 - this->gridStartZ) * 8);
                            blockPos1 = blockPos1.offset(rotation.rotateFacing(EnumFacing::EAST), (j1 - this->gridStartX) * 8);
                            pieces.emplace_back(this->templates, "corridor_floor", blockPos1, rotation);

                            if (woodlandMansionPieces$simpleGrid3.get(j1, i1 - 1) == 1 || (woodlandMansionPieces$simpleGrid2.get(j1, i1 - 1) & 8388608) == 8388608) {
                                pieces.emplace_back(this->templates, "carpet_north", blockPos1.offset(rotation.rotateFacing(EnumFacing::EAST), 1).up(), rotation);
                            }

                            if (woodlandMansionPieces$simpleGrid3.get(j1 + 1, i1) == 1 || (woodlandMansionPieces$simpleGrid2.get(j1 + 1, i1) & 8388608) == 8388608) {
                                pieces.emplace_back(this->templates, "carpet_east", blockPos1.offset(rotation.rotateFacing(EnumFacing::SOUTH), 1).offset(rotation.rotateFacing(EnumFacing::EAST), 5).up(), rotation);
                            }

                            if (woodlandMansionPieces$simpleGrid3.get(j1, i1 + 1) == 1 || (woodlandMansionPieces$simpleGrid2.get(j1, i1 + 1) & 8388608) == 8388608) {
                                pieces.emplace_back(this->templates, s, blockPos1.offset(rotation.rotateFacing(EnumFacing::SOUTH), 5).offset(rotation.rotateFacing(EnumFacing::WEST), 1), rotation);
                            }

                            if (woodlandMansionPieces$simpleGrid3.get(j1 - 1, i1) == 1 || (woodlandMansionPieces$simpleGrid2.get(j1 - 1, i1) & 8388608) == 8388608) {
                                pieces.emplace_back(this->templates, s1, blockPos1.offset(rotation.rotateFacing(EnumFacing::WEST), 1).offset(rotation.rotateFacing(EnumFacing::NORTH), 1), rotation);
                            }
                        }
                    }
                }


                std::cout << "\n=== FLOOR " << l2 << " CORRIDOR vs ROOM PLACEMENT ===" << std::endl;
                for (int i1 = 0; i1 < woodlandMansionPieces$simpleGrid3.height; ++i1) {
                    for (int j1 = 0; j1 < woodlandMansionPieces$simpleGrid3.width; ++j1) {
                        int baseVal = woodlandMansionPieces$simpleGrid3.get(j1, i1);
                        int floorVal = woodlandMansionPieces$simpleGrid2.get(j1, i1);

                        if (baseVal == 1) {  // Corridor
                            Pos3D worldPos = blockPos.offset(rotation.rotateFacing(EnumFacing::SOUTH), 8 + (i1 - this->gridStartZ) * 8);
                            worldPos = worldPos.offset(rotation.rotateFacing(EnumFacing::EAST), (j1 - this->gridStartX) * 8);
                            std::cout << "  Corridor at grid[" << j1 << "," << i1 << "] -> world("
                                      << worldPos.getX() << "," << worldPos.getY() << "," << worldPos.getZ() << ")" << std::endl;
                        }

                        if ((floorVal & 983040) != 0) {  // Has room data
                            int roomType = floorVal & 983040;
                            Pos3D roomPos = blockPos.offset(rotation.rotateFacing(EnumFacing::SOUTH), 8 + (i1 - this->gridStartZ) * 8);
                            roomPos = roomPos.offset(rotation.rotateFacing(EnumFacing::EAST), -1 + (j1 - this->gridStartX) * 8);
                            std::cout << "  Room (type 0x" << std::hex << roomType << std::dec
                                      << ") at grid[" << j1 << "," << i1 << "] -> world("
                                      << roomPos.getX() << "," << roomPos.getY() << "," << roomPos.getZ() << ")"
                                      << " [base value: " << baseVal << "]" << std::endl;
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
                                for (EnumFacing enumFacing : FACING_HORIZONTAL) {
                                    if (woodlandMansionPieces$simpleGrid3.get(l1 + getFrontOffsetX(enumFacing), k1 + getFrontOffsetZ(enumFacing)) == 1) {
                                        list.push_back(enumFacing);
                                    }
                                }
                            }

                            std::optional<EnumFacing> enumFacing1;

                            if (!list.empty()) {
                                auto it = list.begin();
                                std::advance(it, random.nextInt(list.size()));
                                enumFacing1 = *it;
                            } else if (i2 & DOOR_POSITION) {
                                enumFacing1 = EnumFacing::UP;
                            } else {
                                enumFacing1 = std::nullopt;
                            }

                            auto isDir = [&](EnumFacing f){ return enumFacing1 && *enumFacing1 == f; };

                            Pos3D blockPos2 = blockPos.offset(rotation.rotateFacing(EnumFacing::SOUTH), 8 + (k1 - this->gridStartZ) * 8);
                            blockPos2 = blockPos2.offset(rotation.rotateFacing(EnumFacing::EAST), -1 + (l1 - this->gridStartX) * 8);

                            if (Grid::isRoomOrHall(woodlandMansionPieces$simpleGrid3, l1 - 1, k1) && !grid.hasRoomId(woodlandMansionPieces$simpleGrid3, l1 - 1, k1, l2, k2)) {
                                pieces.emplace_back(this->templates, isDir(EnumFacing::WEST) ? s3 : s2, blockPos2, rotation);
                            }

                            if (woodlandMansionPieces$simpleGrid3.get(l1 + 1, k1) == 1 && !flag1) {
                                Pos3D blockPos3 = blockPos2.offset(rotation.rotateFacing(EnumFacing::EAST), 8);
                                pieces.emplace_back(this->templates, isDir(EnumFacing::EAST) ? s3 : s2, blockPos3, rotation);
                            }

                            if (Grid::isRoomOrHall(woodlandMansionPieces$simpleGrid3, l1, k1 + 1) && !grid.hasRoomId(woodlandMansionPieces$simpleGrid3, l1, k1 + 1, l2, k2)) {
                                Pos3D blockPos4 = blockPos2.offset(rotation.rotateFacing(EnumFacing::SOUTH), 7);
                                blockPos4 = blockPos4.offset(rotation.rotateFacing(EnumFacing::EAST), 7);
                                pieces.emplace_back(this->templates, isDir(EnumFacing::SOUTH) ? s3 : s2, blockPos4, rotation.add(Rotation::CLOCKWISE_90));
                            }

                            if (woodlandMansionPieces$simpleGrid3.get(l1, k1 - 1) == 1 && !flag1) {
                                Pos3D blockPos5 = blockPos2.offset(rotation.rotateFacing(EnumFacing::NORTH), 1);
                                blockPos5 = blockPos5.offset(rotation.rotateFacing(EnumFacing::EAST), 7);
                                pieces.emplace_back(this->templates, isDir(EnumFacing::NORTH) ? s3 : s2, blockPos5, rotation.add(Rotation::CLOCKWISE_90));
                            }

                            if (j2 == 65536) {
                                this->place1x1Room(pieces, blockPos2, rotation, enumFacing1, aWoodlandMansionPieces$roomCollection[l2]);
                            } else if (j2 == 131072 && enumFacing1 != std::nullopt) {
                                auto hallDir = grid.findNeighborWithId(woodlandMansionPieces$simpleGrid3, l1, k1, l2, k2);
                                assert(hallDir && "1x2 room expects valid hallDir (Java would NPE)");

                                bool flag2 = (i2 & 4194304) == 4194304;
                                this->place1x2Room(pieces, blockPos2, rotation, *hallDir, enumFacing1.value(), *aWoodlandMansionPieces$roomCollection[l2], flag2);
                            } else if (j2 == 262144 && enumFacing1 != std::nullopt && enumFacing1 != EnumFacing::UP) {
                                EnumFacing enumFacing2 = rotateY(enumFacing1.value());

                                if (!grid.hasRoomId(woodlandMansionPieces$simpleGrid3, l1 + getFrontOffsetX(enumFacing2), k1 + getFrontOffsetZ(enumFacing2), l2, k2)) {
                                    enumFacing2 = getOppositeFacing(enumFacing2);
                                }

                                this->place2x2Room(pieces, blockPos2, rotation, enumFacing2, enumFacing1.value(), *aWoodlandMansionPieces$roomCollection[l2]);
                            } else if (j2 == 262144 && enumFacing1 == EnumFacing::UP) {
                                this->placeStairsRoom(pieces, blockPos2, rotation, *aWoodlandMansionPieces$roomCollection[l2]);
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
                if (!Grid::isRoomOrHall(grid, i + getFrontOffsetX(dir), j + getFrontOffsetZ(dir))) {
                    this->turnRightCorner(out, pd);
                    dir = rotateY(dir);

                    if (i != endX || j != endZ || enumFacing != dir) {
                        this->placeWallPanel(out, pd);
                    }
                } else if (Grid::isRoomOrHall(grid, i + getFrontOffsetX(dir), j + getFrontOffsetZ(dir)) &&
                           Grid::isRoomOrHall(grid, i + getFrontOffsetX(dir) + getFrontOffsetX(rotateYCCW(dir)), j + getFrontOffsetZ(dir) + getFrontOffsetZ(rotateYCCW(dir)))) {
                    turnLeftCorner(out, pd);
                    i += getFrontOffsetX(dir);
                    j += getFrontOffsetZ(dir);
                    dir = rotateYCCW(dir);
                } else {
                    i += getFrontOffsetX(dir);
                    j += getFrontOffsetZ(dir);

                    if (i != endX || j != endZ || enumFacing != dir) {
                        this->placeWallPanel(out, pd);
                    }
                }

                if (i == endX && j == endZ && enumFacing == dir) {
                    break;
                }
            }
        }

        void placeRoofs(std::list<MansionTemplate>& out, Pos3D roofBase, Rotation rot, SimpleGrid& grid, SimpleGrid* upper) {
            for (int i = 0; i < grid.height; ++i) {
                for (int j = 0; j < grid.width; ++j) {
                    Pos3D lvt_8_3_ = roofBase.offset(rot.rotateFacing(EnumFacing::SOUTH), 8 + (i - this->gridStartZ) * 8);
                    lvt_8_3_ = lvt_8_3_.offset(rot.rotateFacing(EnumFacing::EAST), (j - this->gridStartX) * 8);
                    bool flag = upper != nullptr && Grid::isRoomOrHall(*upper, j, i);

                    if (Grid::isRoomOrHall(grid, j, i) && !flag) {
                        out.emplace_back(this->templates, "roof", lvt_8_3_.up(3), rot);

                        if (!Grid::isRoomOrHall(grid, j + 1, i)) {
                            Pos3D blockPos1 = lvt_8_3_.offset(rot.rotateFacing(EnumFacing::EAST), 6);
                            out.emplace_back(this->templates, "roof_front", blockPos1, rot);
                        }

                        if (!Grid::isRoomOrHall(grid, j - 1, i)) {
                            Pos3D blockPos5 = lvt_8_3_.offset(rot.rotateFacing(EnumFacing::EAST), 0);
                            blockPos5 = blockPos5.offset(rot.rotateFacing(EnumFacing::SOUTH), 7);
                            out.emplace_back(this->templates, "roof_front", blockPos5, rot.add(Rotation::CLOCKWISE_180));
                        }

                        if (!Grid::isRoomOrHall(grid, j, i - 1)) {
                            Pos3D blockPos6 = lvt_8_3_.offset(rot.rotateFacing(EnumFacing::WEST), 1);
                            out.emplace_back(this->templates, "roof_front", blockPos6, rot.add(Rotation::COUNTERCLOCKWISE_90));
                        }

                        if (!Grid::isRoomOrHall(grid, j, i + 1)) {
                            Pos3D blockPos7 = lvt_8_3_.offset(rot.rotateFacing(EnumFacing::EAST), 6);
                            blockPos7 = blockPos7.offset(rot.rotateFacing(EnumFacing::SOUTH), 6);
                            out.emplace_back(this->templates, "roof_front", blockPos7, rot.add(Rotation::CLOCKWISE_90));
                        }
                    }
                }
            }

            if (upper != nullptr) {
                for (int k = 0; k < grid.height; ++k) {
                    for (int i1 = 0; i1 < grid.width; ++i1) {
                        Pos3D blockPos3 = roofBase.offset(rot.rotateFacing(EnumFacing::SOUTH), 8 + (k - this->gridStartZ) * 8);
                        blockPos3 = blockPos3.offset(rot.rotateFacing(EnumFacing::EAST), (i1 - this->gridStartX) * 8);
                        bool flag1 = Grid::isRoomOrHall(*upper, i1, k);

                        if (Grid::isRoomOrHall(grid, i1, k) && flag1) {
                            if (!Grid::isRoomOrHall(grid, i1 + 1, k)) {
                                Pos3D blockPos8 = blockPos3.offset(rot.rotateFacing(EnumFacing::EAST), 7);
                                out.emplace_back(this->templates, "small_wall", blockPos8, rot);
                            }

                            if (!Grid::isRoomOrHall(grid, i1 - 1, k)) {
                                Pos3D blockPos9 = blockPos3.offset(rot.rotateFacing(EnumFacing::WEST), 1);
                                blockPos9 = blockPos9.offset(rot.rotateFacing(EnumFacing::SOUTH), 6);
                                out.emplace_back(this->templates, "small_wall", blockPos9, rot.add(Rotation::CLOCKWISE_180));
                            }

                            if (!Grid::isRoomOrHall(grid, i1, k - 1)) {
                                Pos3D blockPos10 = blockPos3.offset(rot.rotateFacing(EnumFacing::WEST), 0);
                                blockPos10 = blockPos10.offset(rot.rotateFacing(EnumFacing::NORTH), 1);
                                out.emplace_back(this->templates, "small_wall", blockPos10, rot.add(Rotation::COUNTERCLOCKWISE_90));
                            }

                            if (!Grid::isRoomOrHall(grid, i1, k + 1)) {
                                Pos3D blockPos11 = blockPos3.offset(rot.rotateFacing(EnumFacing::EAST), 6);
                                blockPos11 = blockPos11.offset(rot.rotateFacing(EnumFacing::SOUTH), 7);
                                out.emplace_back(this->templates, "small_wall", blockPos11, rot.add(Rotation::CLOCKWISE_90));
                            }

                            if (!Grid::isRoomOrHall(grid, i1 + 1, k)) {
                                if (!Grid::isRoomOrHall(grid, i1, k - 1)) {
                                    Pos3D blockPos12 = blockPos3.offset(rot.rotateFacing(EnumFacing::EAST), 7);
                                    blockPos12 = blockPos12.offset(rot.rotateFacing(EnumFacing::NORTH), 2);
                                    out.emplace_back(this->templates, "small_wall_corner", blockPos12, rot);
                                }

                                if (!Grid::isRoomOrHall(grid, i1, k + 1)) {
                                    Pos3D blockPos13 = blockPos3.offset(rot.rotateFacing(EnumFacing::EAST), 8);
                                    blockPos13 = blockPos13.offset(rot.rotateFacing(EnumFacing::SOUTH), 7);
                                    out.emplace_back(this->templates, "small_wall_corner", blockPos13, rot.add(Rotation::CLOCKWISE_90));
                                }
                            }

                            if (!Grid::isRoomOrHall(grid, i1 - 1, k)) {
                                if (!Grid::isRoomOrHall(grid, i1, k - 1)) {
                                    Pos3D blockPos14 = blockPos3.offset(rot.rotateFacing(EnumFacing::WEST), 2);
                                    blockPos14 = blockPos14.offset(rot.rotateFacing(EnumFacing::NORTH), 1);
                                    out.emplace_back(this->templates, "small_wall_corner", blockPos14, rot.add(Rotation::COUNTERCLOCKWISE_90));
                                }

                                if (!Grid::isRoomOrHall(grid, i1, k + 1)) {
                                    Pos3D blockPos15 = blockPos3.offset(rot.rotateFacing(EnumFacing::WEST), 1);
                                    blockPos15 = blockPos15.offset(rot.rotateFacing(EnumFacing::SOUTH), 8);
                                    out.emplace_back(this->templates, "small_wall_corner", blockPos15, rot.add(Rotation::CLOCKWISE_180));
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
                    bool flag2 = upper != nullptr && Grid::isRoomOrHall(*upper, j1, l);

                    if (Grid::isRoomOrHall(grid, j1, l) && !flag2) {
                        if (!Grid::isRoomOrHall(grid, j1 + 1, l)) {
                            Pos3D blockPos16 = blockPos4.offset(rot.rotateFacing(EnumFacing::EAST), 6);

                            if (!Grid::isRoomOrHall(grid, j1, l + 1)) {
                                Pos3D blockPos2 = blockPos16.offset(rot.rotateFacing(EnumFacing::SOUTH), 6);
                                out.emplace_back(this->templates, "roof_corner", blockPos2, rot);
                            } else if (Grid::isRoomOrHall(grid, j1 + 1, l + 1)) {
                                Pos3D blockPos18 = blockPos16.offset(rot.rotateFacing(EnumFacing::SOUTH), 5);
                                out.emplace_back(this->templates, "roof_inner_corner", blockPos18, rot);
                            }

                            if (!Grid::isRoomOrHall(grid, j1, l - 1)) {
                                out.emplace_back(this->templates, "roof_corner", blockPos16, rot.add(Rotation::COUNTERCLOCKWISE_90));
                            } else if (Grid::isRoomOrHall(grid, j1 + 1, l - 1)) {
                                Pos3D blockPos19 = blockPos4.offset(rot.rotateFacing(EnumFacing::EAST), 9);
                                blockPos19 = blockPos19.offset(rot.rotateFacing(EnumFacing::NORTH), 2);
                                out.emplace_back(this->templates, "roof_inner_corner", blockPos19, rot.add(Rotation::CLOCKWISE_90));
                            }
                        }

                        if (!Grid::isRoomOrHall(grid, j1 - 1, l)) {
                            Pos3D blockPos17 = blockPos4.offset(rot.rotateFacing(EnumFacing::EAST), 0);
                            blockPos17 = blockPos17.offset(rot.rotateFacing(EnumFacing::SOUTH), 0);

                            if (!Grid::isRoomOrHall(grid, j1, l + 1)) {
                                Pos3D blockPos20 = blockPos17.offset(rot.rotateFacing(EnumFacing::SOUTH), 6);
                                out.emplace_back(this->templates, "roof_corner", blockPos20, rot.add(Rotation::CLOCKWISE_90));
                            } else if (Grid::isRoomOrHall(grid, j1 - 1, l + 1)) {
                                Pos3D blockPos21 = blockPos17.offset(rot.rotateFacing(EnumFacing::SOUTH), 8);
                                blockPos21 = blockPos21.offset(rot.rotateFacing(EnumFacing::WEST), 3);
                                out.emplace_back(this->templates, "roof_inner_corner", blockPos21, rot.add(Rotation::COUNTERCLOCKWISE_90));
                            }

                            if (!Grid::isRoomOrHall(grid, j1, l - 1)) {
                                out.emplace_back(this->templates, "roof_corner", blockPos17, rot.add(Rotation::CLOCKWISE_180));
                            } else if (Grid::isRoomOrHall(grid, j1 - 1, l - 1)) {
                                Pos3D blockPos22 = blockPos17.offset(rot.rotateFacing(EnumFacing::SOUTH), 1);
                                out.emplace_back(this->templates, "roof_inner_corner", blockPos22, rot.add(Rotation::CLOCKWISE_180));
                            }
                        }
                    }
                }
            }
        }

        void addEntrance(std::list<MansionTemplate>& out, PlacementData& pd) {
            const EnumFacing enumFacing = pd.rotation.rotateFacing(EnumFacing::WEST);
            out.emplace_back(this->templates, "entrance", pd.pos.offset(enumFacing, 9), pd.rotation);
            pd.pos = pd.pos.offset(pd.rotation.rotateFacing(EnumFacing::SOUTH), 16);
        }

        void placeWallPanel(std::list<MansionTemplate>& out, PlacementData& pd) {
            out.emplace_back(this->templates, pd.wallKey, pd.pos.offset(pd.rotation.rotateFacing(EnumFacing::EAST), 7), pd.rotation);
            pd.pos = pd.pos.offset(pd.rotation.rotateFacing(EnumFacing::SOUTH), 8);
        }

        void turnRightCorner(std::list<MansionTemplate>& out, PlacementData& pd) {
            pd.pos = pd.pos.offset(pd.rotation.rotateFacing(EnumFacing::SOUTH), -1);
            out.emplace_back(this->templates, "wall_corner", pd.pos, pd.rotation);
            pd.pos = pd.pos.offset(pd.rotation.rotateFacing(EnumFacing::SOUTH), -7);
            pd.pos = pd.pos.offset(pd.rotation.rotateFacing(EnumFacing::WEST), -6);
            pd.rotation = pd.rotation.add(Rotation::CLOCKWISE_90);
        }

        static void turnLeftCorner(std::list<MansionTemplate>& out, PlacementData& pd) {
            pd.pos = pd.pos.offset(pd.rotation.rotateFacing(EnumFacing::SOUTH), 6);
            pd.pos = pd.pos.offset(pd.rotation.rotateFacing(EnumFacing::EAST), 8);
            pd.rotation = pd.rotation.add(Rotation::COUNTERCLOCKWISE_90);
        }

        static Pos3D func_190942_a(Pos3D blockPos, Rotation rot) {
            switch (rot.type) {
                case Rotation::Type::NONE:
                default:
                    return blockPos;

                case Rotation::Type::CLOCKWISE_90:
                    return {-blockPos.getZ(), blockPos.getY(), blockPos.getX()};

                case Rotation::Type::CLOCKWISE_180:
                    return {-blockPos.getX(), blockPos.getY(), -blockPos.getZ()};

                case Rotation::Type::COUNTERCLOCKWISE_90:
                    return {blockPos.getZ(), blockPos.getY(), -blockPos.getX()};
            }
        }

        void place1x1Room(std::list<MansionTemplate>& out, const Pos3D basePos, Rotation baseRot, const std::optional<EnumFacing> doorDir, RoomCollection* rooms) {
            Rotation rotation = Rotation::NONE;
            std::string s = rooms->get1x1Room(this->random);

            if (!doorDir || *doorDir != EnumFacing::EAST) {

                if (doorDir && *doorDir == EnumFacing::NORTH) {
                    rotation = rotation.add(Rotation::COUNTERCLOCKWISE_90);

                } else if (doorDir && *doorDir == EnumFacing::WEST) {
                    rotation = rotation.add(Rotation::CLOCKWISE_180);

                } else if (doorDir && *doorDir == EnumFacing::SOUTH) {
                    rotation = rotation.add(Rotation::CLOCKWISE_90);

                } else {
                    // Java: else { s = get1x1Secret() }
                    // This is what happens when doorDir == null or UP
                    s = rooms->get1x1Secret(this->random);
                }
            }

            Pos3D blockPos = Template::transformOrigin({1, 0, 0}, Mirror::NONE, rotation, 7, 7);
            rotation = rotation.add(baseRot);
            blockPos = func_190942_a(blockPos, baseRot);
            Pos3D blockPos1 = basePos.add(blockPos.getX(), 0, blockPos.getZ());
            out.emplace_back(this->templates, s, blockPos1, rotation);
        }

        void place1x2Room(std::list<MansionTemplate>& out, const Pos3D basePos, Rotation baseRot, EnumFacing hallDir, const EnumFacing doorDir, RoomCollection& rooms, const bool withStairs) {
            // EnumFacing hallDir

            if (doorDir == EnumFacing::EAST && hallDir == EnumFacing::SOUTH) {
                Pos3D blockPos13 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 1);
                out.emplace_back(this->templates, rooms.get1x2A(this->random, withStairs), blockPos13, baseRot);
            } else if (doorDir == EnumFacing::EAST && hallDir == EnumFacing::NORTH) {
                Pos3D blockPos12 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 1);
                blockPos12 = blockPos12.offset(baseRot.rotateFacing(EnumFacing::SOUTH), 6);
                out.emplace_back(this->templates, rooms.get1x2A(this->random, withStairs), blockPos12, baseRot, Mirror::LEFT_RIGHT);
            } else if (doorDir == EnumFacing::WEST && hallDir == EnumFacing::NORTH) {
                Pos3D blockPos11 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 7);
                blockPos11 = blockPos11.offset(baseRot.rotateFacing(EnumFacing::SOUTH), 6);
                out.emplace_back(this->templates, rooms.get1x2A(this->random, withStairs), blockPos11, baseRot.add(Rotation::CLOCKWISE_180));
            } else if (doorDir == EnumFacing::WEST && hallDir == EnumFacing::SOUTH) {
                Pos3D blockPos10 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 7);
                out.emplace_back(this->templates, rooms.get1x2A(this->random, withStairs), blockPos10, baseRot, Mirror::FRONT_BACK);
            } else if (doorDir == EnumFacing::SOUTH && hallDir == EnumFacing::EAST) {
                Pos3D blockPos9 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 1);
                out.emplace_back(this->templates, rooms.get1x2A(this->random, withStairs), blockPos9, baseRot.add(Rotation::CLOCKWISE_90), Mirror::LEFT_RIGHT);
            } else if (doorDir == EnumFacing::SOUTH && hallDir == EnumFacing::WEST) {
                Pos3D blockPos8 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 7);
                out.emplace_back(this->templates, rooms.get1x2A(this->random, withStairs), blockPos8, baseRot.add(Rotation::CLOCKWISE_90));
            } else if (doorDir == EnumFacing::NORTH && hallDir == EnumFacing::WEST) {
                Pos3D blockPos7 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 7);
                blockPos7 = blockPos7.offset(baseRot.rotateFacing(EnumFacing::SOUTH), 6);
                out.emplace_back(this->templates, rooms.get1x2A(this->random, withStairs), blockPos7, baseRot.add(Rotation::CLOCKWISE_90), Mirror::FRONT_BACK);
            } else if (doorDir == EnumFacing::NORTH && hallDir == EnumFacing::EAST) {
                Pos3D blockPos6 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 1);
                blockPos6 = blockPos6.offset(baseRot.rotateFacing(EnumFacing::SOUTH), 6);
                out.emplace_back(this->templates, rooms.get1x2A(this->random, withStairs), blockPos6, baseRot.add(Rotation::COUNTERCLOCKWISE_90));
            } else if (doorDir == EnumFacing::SOUTH && hallDir == EnumFacing::NORTH) {
                Pos3D blockPos5 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 1);
                blockPos5 = blockPos5.offset(baseRot.rotateFacing(EnumFacing::NORTH), 8);
                out.emplace_back(this->templates, rooms.get1x2B(this->random, withStairs), blockPos5, baseRot);
            } else if (doorDir == EnumFacing::NORTH && hallDir == EnumFacing::SOUTH) {
                Pos3D blockPos4 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 7);
                blockPos4 = blockPos4.offset(baseRot.rotateFacing(EnumFacing::SOUTH), 14);
                out.emplace_back(this->templates, rooms.get1x2B(this->random, withStairs), blockPos4, baseRot.add(Rotation::CLOCKWISE_180));
            } else if (doorDir == EnumFacing::WEST && hallDir == EnumFacing::EAST) {
                Pos3D blockPos3 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 15);
                out.emplace_back(this->templates, rooms.get1x2B(this->random, withStairs), blockPos3, baseRot.add(Rotation::CLOCKWISE_90));
            } else if (doorDir == EnumFacing::EAST && hallDir == EnumFacing::WEST) {
                Pos3D blockPos2 = basePos.offset(baseRot.rotateFacing(EnumFacing::WEST), 7);
                blockPos2 = blockPos2.offset(baseRot.rotateFacing(EnumFacing::SOUTH), 6);
                out.emplace_back(this->templates, rooms.get1x2B(this->random, withStairs), blockPos2, baseRot.add(Rotation::COUNTERCLOCKWISE_90));
            } else if (doorDir == EnumFacing::UP && hallDir == EnumFacing::EAST) {
                Pos3D blockPos1 = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 15);
                out.emplace_back(this->templates, rooms.get1x2StairsEnd(this->random), blockPos1, baseRot.add(Rotation::CLOCKWISE_90));
            } else if (doorDir == EnumFacing::UP && hallDir == EnumFacing::SOUTH) {
                Pos3D blockPos = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 1);
                blockPos = blockPos.offset(baseRot.rotateFacing(EnumFacing::NORTH), 0);
                out.emplace_back(this->templates, rooms.get1x2StairsEnd(this->random), blockPos, baseRot);
            }
        }

        void place2x2Room(std::list<MansionTemplate>& out, const Pos3D basePos, const Rotation baseRot,
                          const EnumFacing cornerDir, const EnumFacing doorDir, RoomCollection& rooms) {
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
            out.emplace_back(this->templates, rooms.get2x2Room(this->random), blockPos, rotation, mirror);
        }

        void placeStairsRoom(std::list<MansionTemplate>& out, Pos3D basePos, Rotation baseRot, RoomCollection& rooms) {
            Pos3D blockPos = basePos.offset(baseRot.rotateFacing(EnumFacing::EAST), 1);
            out.emplace_back(this->templates, rooms.get2x2Secret(this->random), blockPos, baseRot, Mirror::NONE);
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















