#pragma once

#include <string>
#include <utility>

#include "itemTypes.hpp"
#include "itemID.hpp"
#include "LegacyCubiomes/cubiomes/processor.hpp"

#ifdef INCLUDE_QT
#include <QDebug>
#endif


namespace Items {

    struct EntityEquipmentSlot {
        static constexpr int NONE = -1;
        static constexpr int FEET = 0;
        static constexpr int LEGS = 1;
        static constexpr int CHEST = 2;
        static constexpr int HEAD = 3;
    };

    class Item {
    public:
        const int id{};
        const int dataTag{};
        const ItemType type{};
        const std::string name;
        const std::string identifier;
        const bool damageable{};
        const int armorType = EntityEquipmentSlot::NONE;
        const int enchantability = 1;

        Item() = default;

        // could probably be refactored but idc

        // 6 args
        Item(int id, char dataTag, ItemType type, std::string itemName, std::string identifier, bool damageable = false)
                : id(id), dataTag(dataTag), type(type), name(std::move(itemName)), identifier(std::move(identifier)), damageable(damageable) {
        }

        Item(int id, char dataTag, ItemType type, std::string itemName, std::string identifier, bool damageable, int enchantability)
                : id(id), dataTag(dataTag), type(type), name(std::move(itemName)), identifier(std::move(identifier)), damageable(damageable), enchantability(enchantability) {
        }

        Item(int id, char dataTag, ItemType type, std::string itemName, std::string identifier, int enchantability)
                : id(id), dataTag(dataTag), type(type), name(std::move(itemName)), identifier(std::move(identifier)), enchantability(enchantability) {
        }

        Item(int id, char dataTag, ItemType type, std::string itemName, std::string identifier, int armorType, int enchantability)
                : id(id), dataTag(dataTag), type(type), name(std::move(itemName)), identifier(std::move(identifier)), armorType(armorType), enchantability(enchantability) {
        }

        // 4 args
        Item(int id, char dataTag, std::string itemName, std::string  identifier)
                : id(id), dataTag(dataTag), type(ItemType::ItemBlock), name(std::move(itemName)), identifier(std::move(identifier)) {
        }

        explicit operator int() const {
            return id;
        }

        ND std::string getName() const {
            return name;
        }

        ND bool isDamageable() const {
            return damageable;
        }

        ND int getCost() const {
            return enchantability;
        }

        friend std::ostream& operator<<(std::ostream& out, MU const Item &item) {
            out << item.name;
            return out;
        }

        #ifdef INCLUDE_QT
        friend QDebug operator<<(QDebug out, MU const Item &item) {
            out.nospace() << item.name;
            return out.nospace();
        }
        #endif

    };

    class Armor : public Item {
    public:

        // represents enchantability for a specific material
        struct Materials {
            static constexpr int LEATHER = 15;
            static constexpr int CHAIN = 12;
            static constexpr int IRON = 9;
            static constexpr int GOLD = 25;
            static constexpr int DIAMOND = 1;
        };

        Armor(int id, std::string itemName, std::string ident, int slot, int materialEnchantability)
                : Item(id, 0, ItemArmor, std::move(itemName), std::move(ident), slot, materialEnchantability) {
        }
    };

    class Tool : public Item {
    public:

        // represents enchantability for a specific material
        struct Materials {
            static constexpr int WOOD = 15;
            static constexpr int STONE = 5;
            static constexpr int IRON = 14;
            static constexpr int DIAMOND = 10;
            static constexpr int GOLD = 22;
        };

        Tool(int id, std::string itemName, std::string ident, int materialEnchantability)
                : Item(id, 0, ItemTool, std::move(itemName), std::move(ident), true, materialEnchantability) {
        }
    };

    class Sword : public Item {
    public:

        // represents enchantability for a specific material
        struct MU Materials {
            MU static constexpr int WOOD = 15;
            MU static constexpr int STONE = 5;
            MU static constexpr int IRON = 14;
            MU static constexpr int DIAMOND = 10;
            MU static constexpr int GOLD = 22;
        };

        Sword(int id, std::string itemName, std::string ident, int materialEnchantability)
                : Item(id, 0, ItemSword, std::move(itemName), std::move(ident), true, materialEnchantability) {
        }
    };






    MU const Item AIR = Item(0, 0, "Air", "minecraft:air");
    MU const Item STONE = Item(1, 0, "Stone", "minecraft:stone");
    MU const Item GRANITE = Item(1, 1, "Granite", "minecraft:stone");
    MU const Item POLISHED_GRANITE = Item(1, 2, "Polished Granite", "minecraft:stone");
    MU const Item DIORITE = Item(1, 3, "Diorite", "minecraft:stone");
    MU const Item POLISHED_DIORITE = Item(1, 4, "Polished Diorite", "minecraft:stone");
    MU const Item ANDESITE = Item(1, 5, "Andesite", "minecraft:stone");
    MU const Item POLISHED_ANDESITE = Item(1, 6, "Polished Andesite", "minecraft:stone");

    MU const Item GRASS = Item(2, 0, "Grass", "minecraft:grass");
    MU const Item DIRT = Item(3, 0, "Dirt", "minecraft:dirt");
    MU const Item COARSE_DIRT = Item(3, 1, "Coarse Dirt", "minecraft:dirt");
    MU const Item PODZOL = Item(3, 2, "Podzol", "minecraft:dirt");

    MU const Item COBBLESTONE = Item(4, 0, "Cobblestone", "minecraft:cobblestone");

    MU const Item OAK_WOOD_PLANK = Item(5, 0, "Oak Wood Plank", "minecraft:planks");
    MU const Item SPRUCE_WOOD_PLANK = Item(5, 1, "Spruce Wood Plank", "minecraft:planks");
    MU const Item BIRCH_WOOD_PLANK = Item(5, 2, "Birch Wood Plank", "minecraft:planks");
    MU const Item JUNGLE_WOOD_PLANK = Item(5, 3, "Jungle Wood Plank", "minecraft:planks");
    MU const Item ACACIA_WOOD_PLANK = Item(5, 4, "Acacia Wood Plank", "minecraft:planks");
    MU const Item DARK_OAK_WOOD_PLANK = Item(5, 5, "Dark Oak Wood Plank", "minecraft:planks");

    MU const Item OAK_SAPLING = Item(6, 0, "Oak Sapling", "minecraft:sapling");
    MU const Item SPRUCE_SAPLING = Item(6, 1, "Spruce Sapling", "minecraft:sapling");
    MU const Item BIRCH_SAPLING = Item(6, 2, "Birch Sapling", "minecraft:sapling");
    MU const Item JUNGLE_SAPLING = Item(6, 3, "Jungle Sapling", "minecraft:sapling");
    MU const Item ACACIA_SAPLING = Item(6, 4, "Acacia Sapling", "minecraft:sapling");
    MU const Item DARK_OAK_SAPLING = Item(6, 5, "Dark Oak Sapling", "minecraft:sapling");

    MU const Item BEDROCK = Item(7, 0, "Bedrock", "minecraft:bedrock");
    MU const Item FLOWING_WATER = Item(8, 0, "Flowing Water", "minecraft:flowing_water");
    MU const Item STILL_WATER = Item(9, 0, "Still Water", "minecraft:water");
    MU const Item FLOWING_LAVA = Item(10, 0, "Flowing Lava", "minecraft:flowing_lava");
    MU const Item STILL_LAVA = Item(11, 0, "Still Lava", "minecraft:lava");

    MU const Item SAND = Item(12, 0, "Sand", "minecraft:sand");
    MU const Item RED_SAND = Item(12, 1, "Red Sand", "minecraft:sand");

    MU const Item GRAVEL = Item(13, 0, "Gravel", "minecraft:gravel");
    MU const Item GOLD_ORE = Item(14, 0, "Gold Ore", "minecraft:gold_ore");
    MU const Item IRON_ORE = Item(15, 0, "Iron Ore", "minecraft:iron_ore");
    MU const Item COAL_ORE = Item(16, 0, "Coal Ore", "minecraft:coal_ore");

    MU const Item OAK_WOOD = Item(17, 0, "Oak Wood", "minecraft:log");
    MU const Item SPRUCE_WOOD = Item(17, 1, "Spruce Wood", "minecraft:log");
    MU const Item BIRCH_WOOD = Item(17, 2, "Birch Wood", "minecraft:log");
    MU const Item JUNGLE_WOOD = Item(17, 3, "Jungle Wood", "minecraft:log");

    MU const Item OAK_LEAVES = Item(18, 0, "Oak Leaves", "minecraft:leaves");
    MU const Item SPRUCE_LEAVES = Item(18, 1, "Spruce Leaves", "minecraft:leaves");
    MU const Item BIRCH_LEAVES = Item(18, 2, "Birch Leaves", "minecraft:leaves");
    MU const Item JUNGLE_LEAVES = Item(18, 3, "Jungle Leaves", "minecraft:leaves");

    MU const Item SPONGE = Item(19, 0, "Sponge", "minecraft:sponge");
    MU const Item WET_SPONGE = Item(19, 1, "Wet Sponge", "minecraft:sponge");

    MU const Item GLASS = Item(20, 0, "Glass", "minecraft:glass");
    MU const Item LAPIS_LAZULI_ORE = Item(21, 0, "Lapis Lazuli Ore", "minecraft:lapis_ore");
    MU const Item LAPIS_LAZULI_BLOCK = Item(22, 0, "Lapis Lazuli Block", "minecraft:lapis_block");
    MU const Item DISPENSER = Item(23, 0, "Dispenser", "minecraft:dispenser");

    MU const Item SANDSTONE = Item(24, 0, "Sandstone", "minecraft:sandstone");
    MU const Item CHISELED_SANDSTONE = Item(24, 1, "Chiseled Sandstone", "minecraft:sandstone");
    MU const Item SMOOTH_SANDSTONE = Item(24, 2, "Smooth Sandstone", "minecraft:sandstone");

    MU const Item NOTE_BLOCK = Item(25, 0, "Note Block", "minecraft:noteblock");
    MU const Item BED_BLOCK = Item(26, 0, "Bed Block", "minecraft:bed");
    MU const Item POWERED_RAIL = Item(27, 0, "Powered Rail", "minecraft:golden_rail");
    MU const Item DETECTOR_RAIL = Item(28, 0, "Detector Rail", "minecraft:detector_rail");
    MU const Item STICKY_PISTON = Item(29, 0, "Sticky Piston", "minecraft:sticky_piston");
    MU const Item COBWEB = Item(30, 0, "Cobweb", "minecraft:web");

    MU const Item TALL_GRASS_SHRUB = Item(31, 0, "Shrub", "minecraft:tallgrass");
    MU const Item TALL_GRASS_GRASS = Item(31, 1, "Grass", "minecraft:tallgrass");
    MU const Item TALL_GRASS_FERN = Item(31, 2, "Fern", "minecraft:tallgrass");

    MU const Item DEAD_BUSH = Item(32, 0, "Dead Bush", "minecraft:deadbush");

    MU const Item PISTON = Item(33, 0, "Piston", "minecraft:piston");
    MU const Item PISTON_HEAD = Item(34, 0, "Piston Head", "minecraft:piston_head");

    MU const Item WHITE_WOOL = Item(35, 0, "White Wool", "minecraft:wool");
    MU const Item ORANGE_WOOL = Item(35, 1, "Orange Wool", "minecraft:wool");
    MU const Item MAGENTA_WOOL = Item(35, 2, "Magenta Wool", "minecraft:wool");
    MU const Item LIGHT_BLUE_WOOL = Item(35, 3, "Light Blue Wool", "minecraft:wool");
    MU const Item YELLOW_WOOL = Item(35, 4, "Yellow Wool", "minecraft:wool");
    MU const Item LIME_WOOL = Item(35, 5, "Lime Wool", "minecraft:wool");
    MU const Item PINK_WOOL = Item(35, 6, "Pink Wool", "minecraft:wool");
    MU const Item GRAY_WOOL = Item(35, 7, "Gray Wool", "minecraft:wool");
    MU const Item LIGHT_GRAY_WOOL = Item(35, 8, "Light Gray Wool", "minecraft:wool");
    MU const Item CYAN_WOOL = Item(35, 9, "Cyan Wool", "minecraft:wool");
    MU const Item PURPLE_WOOL = Item(35, 10, "Purple Wool", "minecraft:wool");
    MU const Item BLUE_WOOL = Item(35, 11, "Blue Wool", "minecraft:wool");
    MU const Item BROWN_WOOL = Item(35, 12, "Brown Wool", "minecraft:wool");
    MU const Item GREEN_WOOL = Item(35, 13, "Green Wool", "minecraft:wool");
    MU const Item RED_WOOL = Item(35, 14, "Red Wool", "minecraft:wool");
    MU const Item BLACK_WOOL = Item(35, 15, "Black Wool", "minecraft:wool");

    MU const Item DANDELION = Item(37, 0, "Dandelion", "minecraft:yellow_flower");

    MU const Item POPPY = Item(38, 0, "Poppy", "minecraft:red_flower");
    MU const Item BLUE_ORCHID = Item(38, 1, "Blue Orchid", "minecraft:red_flower");
    MU const Item ALLIUM = Item(38, 2, "Allium", "minecraft:red_flower");
    MU const Item AZURE_BLUET = Item(38, 3, "Azure Bluet", "minecraft:red_flower");
    MU const Item RED_TULIP = Item(38, 4, "Red Tulip", "minecraft:red_flower");
    MU const Item ORANGE_TULIP = Item(38, 5, "Orange Tulip", "minecraft:red_flower");
    MU const Item WHITE_TULIP = Item(38, 6, "White Tulip", "minecraft:red_flower");
    MU const Item PINK_TULIP = Item(38, 7, "Pink Tulip", "minecraft:red_flower");
    MU const Item OXEYE_DAISY = Item(38, 8, "Oxeye Daisy", "minecraft:red_flower");

    MU const Item BROWN_MUSHROOM = Item(39, 0, "Brown Mushroom", "minecraft:brown_mushroom");
    MU const Item RED_MUSHROOM = Item(40, 0, "Red Mushroom", "minecraft:red_mushroom");
    MU const Item GOLD_BLOCK = Item(41, 0, "Gold Block", "minecraft:gold_block");
    MU const Item IRON_BLOCK = Item(42, 0, "Iron Block", "minecraft:iron_block");

    MU const Item DOUBLE_STONE_SLAB = Item(43, 0, "Double Stone Slab", "minecraft:double_stone_slab");
    MU const Item DOUBLE_SANDSTONE_SLAB = Item(43, 1, "Double Sandstone Slab", "minecraft:double_stone_slab");
    MU const Item DOUBLE_WOODEN_SLAB = Item(43, 2, "Double Wooden Slab", "minecraft:double_stone_slab");
    MU const Item DOUBLE_COBBLESTONE_SLAB = Item(43, 3, "Double Cobblestone Slab", "minecraft:double_stone_slab");
    MU const Item DOUBLE_BRICK_SLAB = Item(43, 4, "Double Brick Slab", "minecraft:double_stone_slab");
    MU const Item DOUBLE_STONE_BRICK_SLAB = Item(43, 5, "Double Stone Brick Slab", "minecraft:double_stone_slab");
    MU const Item DOUBLE_NETHER_BRICK_SLAB = Item(43, 6, "Double Nether Brick Slab", "minecraft:double_stone_slab");
    MU const Item DOUBLE_QUARTZ_SLAB = Item(43, 7, "Double Quartz Slab", "minecraft:double_stone_slab");
    MU const Item SMOOTH_DOUBLE_STONE_SLAB = Item(43, 8, "Smooth Double Stone Slab", "minecraft:double_stone_slab");
    MU const Item SMOOTH_DOUBLE_SANDSTONE_SLAB = Item(43, 9, "Smooth Double Sandstone Slab", "minecraft:double_stone_slab");
    MU const Item TILE_DOUBLE_QUARTZ_SLAB = Item(43, 15, "Tile Double Quartz Slab", "minecraft:double_stone_slab");

    MU const Item STONE_SLAB = Item(44, 0, "Stone Slab", "minecraft:stone_slab");
    MU const Item SANDSTONE_SLAB = Item(44, 1, "Sandstone Slab", "minecraft:stone_slab");
    MU const Item WOODEN_SLAB = Item(44, 2, "Wooden Slab", "minecraft:stone_slab");
    MU const Item COBBLESTONE_SLAB = Item(44, 3, "Cobblestone Slab", "minecraft:stone_slab");
    MU const Item BRICK_SLAB = Item(44, 4, "Brick Slab", "minecraft:stone_slab");
    MU const Item STONE_BRICK_SLAB = Item(44, 5, "Stone Brick Slab", "minecraft:stone_slab");
    MU const Item NETHER_STONE_SLAB = Item(44, 6, "Nether Brick Slab", "minecraft:stone_slab");
    MU const Item QUARTZ_SLAB = Item(44, 7, "Quartz Slab", "minecraft:stone_slab");
    MU const Item UPPER_STONE_SLAB = Item(44, 8, "Upper Stone Slab", "minecraft:stone_slab");
    MU const Item UPPER_SANDSTONE_SLAB = Item(44, 9, "Upper Sandstone Slab", "minecraft:stone_slab");
    MU const Item UPPER_WOODEN_SLAB = Item(44, 10, "Upper Wooden Slab", "minecraft:stone_slab");
    MU const Item UPPER_COBBLESTONE_SLAB = Item(44, 11, "Upper Cobblestone Slab", "minecraft:stone_slab");
    MU const Item UPPER_BRICKS_SLAB = Item(44, 12, "Upper Bricks Slab", "minecraft:stone_slab");
    MU const Item UPPER_STONE_BRICK_SLAB = Item(44, 13, "Upper Stone Brick Slab", "minecraft:stone_slab");
    MU const Item UPPER_NETHER_BRICK_SLAB = Item(44, 14, "Upper Nether Brick Slab", "minecraft:stone_slab");
    MU const Item UPPER_QUARTZ_SLAB = Item(44, 15, "Upper Quartz Slab", "minecraft:stone_slab");

    MU const Item BRICKS = Item(45, 0, "Bricks", "minecraft:brick_block");
    MU const Item TNT = Item(46, 0, "TNT", "minecraft:tnt");
    MU const Item BOOKSHELF = Item(47, 0, "Bookshelf", "minecraft:bookshelf");
    MU const Item MOSS_STONE = Item(48, 0, "Moss Stone", "minecraft:mossy_cobblestone");
    MU const Item OBSIDIAN = Item(49, 0, "Obsidian", "minecraft:obsidian");
    MU const Item TORCH = Item(50, 0, "Torch", "minecraft:torch");
    MU const Item FIRE = Item(51, 0, "Fire", "minecraft:fire");
    MU const Item MONSTER_SPAWNER = Item(52, 0, "Monster Spawner", "minecraft:mob_spawner");
    MU const Item OAK_WOOD_STAIRS = Item(53, 0, "Oak Wood Stairs", "minecraft:oak_stairs");
    MU const Item CHEST = Item(54, 0, "Chest", "minecraft:chest");
    MU const Item REDSTONE_WIRE = Item(55, 0, "Redstone Wire", "minecraft:redstone_wire");
    MU const Item DIAMOND_ORE = Item(56, 0, "Diamond Ore", "minecraft:diamond_ore");
    MU const Item DIAMOND_BLOCK = Item(57, 0, "Diamond Block", "minecraft:diamond_block");
    MU const Item CRAFTING_TABLE = Item(58, 0, "Crafting Table", "minecraft:crafting_table");
    MU const Item WHEAT_CROPS = Item(59, 0, "Wheat Crops", "minecraft:wheat");
    MU const Item FARMLAND = Item(60, 0, "Farmland", "minecraft:farmland");
    MU const Item FURNACE = Item(61, 0, "Furnace", "minecraft:furnace");
    MU const Item BURNING_FURNACE = Item(62, 0, "Burning Furnace", "minecraft:lit_furnace");
    MU const Item STANDING_SIGN_BLOCK = Item(63, 0, "Standing Sign Block", "minecraft:standing_sign");
    MU const Item OAK_DOOR_BLOCK = Item(64, 0, "Oak Door Block", "minecraft:wooden_door");
    MU const Item LADDER = Item(65, 0, "Ladder", "minecraft:ladder");
    MU const Item RAIL = Item(66, 0, "Rail", "minecraft:rail");
    MU const Item COBBLESTONE_STAIRS = Item(67, 0, "Cobblestone Stairs", "minecraft:stone_stairs");
    MU const Item WALL_MOUNTED_SIGN_BLOCK = Item(68, 0, "Wall-mounted Sign Block", "minecraft:wall_sign");
    MU const Item LEVER = Item(69, 0, "Lever", "minecraft:lever");
    MU const Item STONE_PRESSURE_PLATE = Item(70, 0, "Stone Pressure Plate", "minecraft:stone_pressure_plate");
    MU const Item IRON_DOOR_BLOCK = Item(71, 0, "Iron Door Block", "minecraft:iron_door");
    MU const Item WOODEN_PRESSURE_PLATE = Item(72, 0, "Wooden Pressure Plate", "minecraft:wooden_pressure_plate");
    MU const Item REDSTONE_ORE = Item(73, 0, "Redstone Ore", "minecraft:redstone_ore");
    MU const Item GLOWING_REDSTONE_ORE = Item(74, 0, "Glowing Redstone Ore", "minecraft:lit_redstone_ore");
    MU const Item OFF_REDSTONE_TORCH = Item(75, 0, "Redstone Torch (off)", "minecraft:unlit_redstone_torch");
    MU const Item ON_REDSTONE_TORCH = Item(76, 0, "Redstone Torch (on)", "minecraft:redstone_torch");
    MU const Item STONE_BUTTON = Item(77, 0, "Stone Button", "minecraft:stone_button");
    MU const Item SNOW = Item(78, 0, "Snow", "minecraft:snow_layer");
    MU const Item ICE = Item(79, 0, "Ice", "minecraft:ice");
    MU const Item SNOW_BLOCK = Item(80, 0, "Snow Block", "minecraft:snow");
    MU const Item CACTUS = Item(81, 0, "Cactus", "minecraft:cactus");
    MU const Item CLAY_BLOCK = Item(82, 0, "Clay", "minecraft:clay");
    MU const Item SUGAR_CANES = Item(83, 0, "Sugar Canes", "minecraft:reeds");
    MU const Item JUKEBOX = Item(84, 0, "Jukebox", "minecraft:jukebox");
    MU const Item OAK_FENCE = Item(85, 0, "Oak Fence", "minecraft:fence");
    MU const Item PUMPKIN = Item(86, 0, "Pumpkin", "minecraft:pumpkin");
    MU const Item NETHERRACK = Item(87, 0, "Netherrack", "minecraft:netherrack");
    MU const Item SOUL_SAND = Item(88, 0, "Soul Sand", "minecraft:soul_sand");
    MU const Item GLOWSTONE = Item(89, 0, "Glowstone", "minecraft:glowstone");
    MU const Item NETHER_PORTAL = Item(90, 0, "Nether Portal", "minecraft:portal");
    MU const Item JACK_O_LANTERN = Item(91, 0, "Jack o’Lantern", "minecraft:lit_pumpkin");
    MU const Item CAKE_BLOCK = Item(92, 0, "Cake Block", "minecraft:cake");
    MU const Item OFF_REDSTONE_REPEATER_BLOCK = Item(93, 0, "Redstone Repeater Block (off)", "minecraft:unpowered_repeater");
    MU const Item ON_REDSTONE_REPEATER_BLOCK = Item(94, 0, "Redstone Repeater Block (on)", "minecraft:powered_repeater");

    MU const Item WHITE_STAINED_GLASS = Item(95, 0, "White Stained Glass", "minecraft:stained_glass");
    MU const Item ORANGE_STAINED_GLASS = Item(95, 1, "Orange Stained Glass", "minecraft:stained_glass");
    MU const Item MAGENTA_STAINED_GLASS = Item(95, 2, "Magenta Stained Glass", "minecraft:stained_glass");
    MU const Item LIGHT_BLUE_STAINED_GLASS = Item(95, 3, "Light Blue Stained Glass", "minecraft:stained_glass");
    MU const Item YELLOW_STAINED_GLASS = Item(95, 4, "Yellow Stained Glass", "minecraft:stained_glass");
    MU const Item LIME_STAINED_GLASS = Item(95, 5, "Lime Stained Glass", "minecraft:stained_glass");
    MU const Item PINK_STAINED_GLASS = Item(95, 6, "Pink Stained Glass", "minecraft:stained_glass");
    MU const Item GRAY_STAINED_GLASS = Item(95, 7, "Gray Stained Glass", "minecraft:stained_glass");
    MU const Item LIGHT_GRAY_STAINED_GLASS = Item(95, 8, "Light Gray Stained Glass", "minecraft:stained_glass");
    MU const Item CYAN_STAINED_GLASS = Item(95, 9, "Cyan Stained Glass", "minecraft:stained_glass");
    MU const Item PURPLE_STAINED_GLASS = Item(95, 10, "Purple Stained Glass", "minecraft:stained_glass");
    MU const Item BLUE_STAINED_GLASS = Item(95, 11, "Blue Stained Glass", "minecraft:stained_glass");
    MU const Item BROWN_STAINED_GLASS = Item(95, 12, "Brown Stained Glass", "minecraft:stained_glass");
    MU const Item GREEN_STAINED_GLASS = Item(95, 13, "Green Stained Glass", "minecraft:stained_glass");
    MU const Item RED_STAINED_GLASS = Item(95, 14, "Red Stained Glass", "minecraft:stained_glass");
    MU const Item BLACK_STAINED_GLASS = Item(95, 15, "Black Stained Glass", "minecraft:stained_glass");

    MU const Item WOODEN_TRAPDOOR = Item(96, 0, "Wooden Trapdoor", "minecraft:trapdoor");

    MU const Item STONE_MONSTER_EGG = Item(97, 0, "Stone Monster Egg", "minecraft:monster_egg");
    MU const Item COBBLESTONE_MONSTER_EGG = Item(97, 1, "Cobblestone Monster Egg", "minecraft:monster_egg");
    MU const Item STONE_BRICK_MONSTER_EGG = Item(97, 2, "Stone Brick Monster Egg", "minecraft:monster_egg");
    MU const Item MOSSY_STONE_MONSTER_EGG = Item(97, 3, "Mossy Stone Brick Monster Egg", "minecraft:monster_egg");
    MU const Item CRACKED_STONE_BRICK_MONSTER_EGG = Item(97, 4, "Cracked Stone Brick Monster Egg", "minecraft:monster_egg");
    MU const Item CHISELED_STONE_BRICK_MONSTER_EGG = Item(97, 5, "Chiseled Stone Brick Monster Egg", "minecraft:monster_egg");

    MU const Item STONE_BRICKS = Item(98, 0, "Stone Bricks", "minecraft:stonebrick");
    MU const Item MOSSY_STONE_BRICKS = Item(98, 1, "Mossy Stone Bricks", "minecraft:stonebrick");
    MU const Item CRACKED_STONE_BRICKS = Item(98, 2, "Cracked Stone Bricks", "minecraft:stonebrick");
    MU const Item CHISELED_STONE_BRICKS = Item(98, 3, "Chiseled Stone Bricks", "minecraft:stonebrick");

    MU const Item BROWN_MUSHROOM_BLOCK = Item(99, 0, "Brown Mushroom Block", "minecraft:brown_mushroom_block");
    MU const Item RED_MUSHROOM_BLOCK = Item(100, 0, "Red Mushroom Block", "minecraft:red_mushroom_block");
    MU const Item IRON_BARS = Item(101, 0, "Iron Bars", "minecraft:iron_bars");
    MU const Item GLASS_PANE = Item(102, 0, "Glass Pane", "minecraft:glass_pane");
    MU const Item MELON_BLOCK = Item(103, 0, "Melon Block", "minecraft:melon_block");
    MU const Item PUMPKIN_STEM = Item(104, 0, "Pumpkin Stem", "minecraft:pumpkin_stem");
    MU const Item MELON_STEM = Item(105, 0, "Melon Stem", "minecraft:melon_stem");
    MU const Item VINES = Item(106, 0, "Vines", "minecraft:vine");
    MU const Item OAK_FENCE_GATE = Item(107, 0, "Oak Fence Gate", "minecraft:fence_gate");
    MU const Item BRICK_STAIRS = Item(108, 0, "Brick Stairs", "minecraft:brick_stairs");
    MU const Item STONE_BRICK_STAIRS = Item(109, 0, "Stone Brick Stairs", "minecraft:stone_brick_stairs");
    MU const Item MYCELIUM = Item(110, 0, "Mycelium", "minecraft:mycelium");
    MU const Item LILY_PAD = Item(111, 0, "Lily Pad", "minecraft:waterlily");
    MU const Item NETHER_BRICK_BLOCK = Item(112, 0, "Nether Brick", "minecraft:nether_brick");
    MU const Item NETHER_BRICK_FENCE = Item(113, 0, "Nether Brick Fence", "minecraft:nether_brick_fence");
    MU const Item NETHER_BRICK_STAIRS = Item(114, 0, "Nether Brick Stairs", "minecraft:nether_brick_stairs");
    MU const Item NETHER_WART = Item(115, 0, "Nether Wart", "minecraft:nether_wart");
    MU const Item ENCHANTMENT_TABLE = Item(116, 0, "Enchantment Table", "minecraft:enchanting_table");
    MU const Item BREWING_STAND = Item(117, 0, "Brewing Stand", "minecraft:brewing_stand");
    MU const Item CAULDRON = Item(118, 0, "Cauldron", "minecraft:cauldron");
    MU const Item END_PORTAL = Item(119, 0, "End Portal", "minecraft:end_portal");
    MU const Item END_PORTAL_FRAME = Item(120, 0, "End Portal Frame", "minecraft:end_portal_frame");
    MU const Item END_STONE = Item(121, 0, "End Stone", "minecraft:end_stone");
    MU const Item DRAGON_EGG = Item(122, 0, "Dragon Egg", "minecraft:dragon_egg");
    MU const Item INACTIVE_REDSTONE_LAMP = Item(123, 0, "Redstone Lamp (inactive)", "minecraft:redstone_lamp");
    MU const Item ACTIVE_REDSTONE_LAMP = Item(124, 0, "Redstone Lamp (active)", "minecraft:lit_redstone_lamp");

    MU const Item DOUBLE_OAK_WOOD_SLAB = Item(125, 0, "Double Oak Wood Slab", "minecraft:double_wooden_slab");
    MU const Item DOUBLE_SPRUCE_WOOD_SLAB = Item(125, 1, "Double Spruce Wood Slab", "minecraft:double_wooden_slab");
    MU const Item DOUBLE_BIRCH_WOOD_SLAB = Item(125, 2, "Double Birch Wood Slab", "minecraft:double_wooden_slab");
    MU const Item DOUBLE_JUNGLE_WOOD_SLAB = Item(125, 3, "Double Jungle Wood Slab", "minecraft:double_wooden_slab");
    MU const Item DOUBLE_ACACIA_WOOD_SLAB = Item(125, 4, "Double Acacia Wood Slab", "minecraft:double_wooden_slab");
    MU const Item DOUBLE_DARK_OAK_WOOD_SLAB = Item(125, 5, "Double Dark Oak Wood Slab", "minecraft:double_wooden_slab");

    MU const Item OAK_WOOD_SLAB = Item(126, 0, "Oak Wood Slab", "minecraft:wooden_slab");
    MU const Item SPRUCE_WOOD_SLAB = Item(126, 1, "Spruce Wood Slab", "minecraft:wooden_slab");
    MU const Item BIRCH_WOOD_SLAB = Item(126, 2, "Birch Wood Slab", "minecraft:wooden_slab");
    MU const Item JUNGLE_WOOD_SLAB = Item(126, 3, "Jungle Wood Slab", "minecraft:wooden_slab");
    MU const Item ACACIA_WOOD_SLAB = Item(126, 4, "Acacia Wood Slab", "minecraft:wooden_slab");
    MU const Item DARK_OAK_WOOD_SLAB = Item(126, 5, "Dark Wood Slab", "minecraft:wooden_slab");
    MU const Item UPPER_OAK_WOOD_SLAB = Item(126, 8, "Upper Oak Wood Slab", "minecraft:wooden_slab");
    MU const Item UPPER_SPRUCE_WOOD_SLAB = Item(126, 9, "Upper Spruce Wood Slab", "minecraft:wooden_slab");
    MU const Item UPPER_BIRCH_WOOD_SLAB = Item(126, 10, "Upper Birch Wood Slab", "minecraft:wooden_slab");
    MU const Item UPPER_JUNGLE_WOOD_SLAB = Item(126, 11, "Upper Jungle Wood Slab", "minecraft:wooden_slab");
    MU const Item UPPER_ACACIA_WOOD_SLAB = Item(126, 12, "Upper Acacia Wood Slab", "minecraft:wooden_slab");
    MU const Item UPPER_DARK_OAK_WOOD_SLAB = Item(126, 13, "Upper Dark Wood Slab", "minecraft:wooden_slab");

    MU const Item COCOA = Item(127, 0, "Cocoa", "minecraft:cocoa");
    MU const Item SANDSTONE_STAIRS = Item(128, 0, "Sandstone Stairs", "minecraft:sandstone_stairs");
    MU const Item EMERALD_ORE = Item(129, 0, "Emerald Ore", "minecraft:emerald_ore");
    MU const Item ENDER_CHEST = Item(130, 0, "Ender Chest", "minecraft:ender_chest");
    MU const Item TRIPWIRE_HOOK = Item(131, 0, "Tripwire Hook", "minecraft:tripwire_hook");
    MU const Item TRIPWIRE = Item(132, 0, "Tripwire", "minecraft:tripwire_hook");
    MU const Item EMERALD_BLOCK = Item(133, 0, "Emerald Block", "minecraft:emerald_block");
    MU const Item SPRUCE_WOOD_STAIRS = Item(134, 0, "Spruce Wood Stairs", "minecraft:spruce_stairs");
    MU const Item BIRCH_WOOD_STAIRS = Item(135, 0, "Birch Wood Stairs", "minecraft:birch_stairs");
    MU const Item JUNGLE_WOOD_STAIRS = Item(136, 0, "Jungle Wood Stairs", "minecraft:jungle_stairs");
    MU const Item COMMAND_BLOCK = Item(137, 0, "Command Block", "minecraft:command_block");
    MU const Item BEACON = Item(138, 0, "Beacon", "minecraft:beacon");

    MU const Item COBBLESTONE_WALL = Item(139, 0, "Cobblestone Wall", "minecraft:cobblestone_wall");
    MU const Item MOSSY_COBBLESTONE_WALL = Item(139, 1, "Mossy Cobblestone Wall", "minecraft:cobblestone_wall");

    MU const Item FLOWER_POT = Item(140, 0, "Flower Pot", "minecraft:flower_pot");
    MU const Item CARROTS = Item(141, 0, "Carrots", "minecraft:carrots");
    MU const Item POTATOES = Item(142, 0, "Potatoes", "minecraft:potatoes");
    MU const Item WOODEN_BUTTON = Item(143, 0, "Wooden Button", "minecraft:wooden_button");

    MU const Item MOB_HEAD = Item(144, 0, "Mob Head", "minecraft:skull");

    MU const Item ANVIL = Item(145, 0, "Anvil", "minecraft:anvil");
    MU const Item SLIGHTLY_DAMAGED_ANVIL = Item(145, 1, "Slightly Damaged Anvil", "minecraft:anvil");
    MU const Item VERY_DAMAGED_ANVIL = Item(145, 2, "Very Damaged Anvil", "minecraft:anvil");

    MU const Item TRAPPED_CHEST = Item(146, 0, "Trapped Chest", "minecraft:trapped_chest");
    MU const Item LIGHT_WEIGHTED_PRESSURE_PLATE = Item(147, 0, "Weighted Pressure Plate (light)", "minecraft:light_weighted_pressure_plate");
    MU const Item HEAVY_WEIGHTED_PRESSURE_PLATE = Item(148, 0, "Weighted Pressure Plate (heavy)", "minecraft:heavy_weighted_pressure_plate");
    MU const Item INACTIVE_REDSTONE_COMPARATOR = Item(149, 0, "Redstone Comparator (inactive)", "minecraft:unpowered_comparator");
    MU const Item ACTIVE_REDSTONE_COMPARATOR = Item(150, 0, "Redstone Comparator (active)", "minecraft:powered_comparator");
    MU const Item DAYLIGHT_SENSOR = Item(151, 0, "Daylight Sensor", "minecraft:daylight_detector");
    MU const Item REDSTONE_BLOCK = Item(152, 0, "Redstone Block", "minecraft:redstone_block");
    MU const Item NETHER_QUARTZ_ORE = Item(153, 0, "Nether Quartz Ore", "minecraft:quartz_ore");
    MU const Item HOPPER = Item(154, 0, "Hopper", "minecraft:hopper");

    MU const Item QUARTZ_BLOCK = Item(155, 0, "Quartz Block", "minecraft:quartz_block");
    MU const Item CHISELED_QUARTZ_BLOCK = Item(155, 1, "Chiseled Quartz Block", "minecraft:quartz_block");
    MU const Item PILLAR_QUARTZ_BLOCK = Item(155, 2, "Pillar Quartz Block", "minecraft:quartz_block");

    MU const Item QUARTZ_STAIRS = Item(156, 0, "Quartz Stairs", "minecraft:quartz_stairs");
    MU const Item ACTIVATOR_RAIL = Item(157, 0, "Activator Rail", "minecraft:activator_rail");
    MU const Item DROPPER = Item(158, 0, "Dropper", "minecraft:dropper");

    MU const Item WHITE_HARDENED_CLAY = Item(159, 0, "White Hardened Clay", "minecraft:stained_hardened_clay");
    MU const Item ORANGE_HARDENED_CLAY = Item(159, 1, "Orange Hardened Clay", "minecraft:stained_hardened_clay");
    MU const Item MAGENTA_HARDENED_CLAY = Item(159, 2, "Magenta Hardened Clay", "minecraft:stained_hardened_clay");
    MU const Item LIGHT_BLUE_HARDENED_CLAY = Item(159, 3, "Light Blue Hardened Clay", "minecraft:stained_hardened_clay");
    MU const Item YELLOW_HARDENED_CLAY = Item(159, 4, "Yellow Hardened Clay", "minecraft:stained_hardened_clay");
    MU const Item LIME_HARDENED_CLAY = Item(159, 5, "Lime Hardened Clay", "minecraft:stained_hardened_clay");
    MU const Item PINK_HARDENED_CLAY = Item(159, 6, "Pink Hardened Clay", "minecraft:stained_hardened_clay");
    MU const Item GRAY_HARDENED_CLAY = Item(159, 7, "Gray Hardened Clay", "minecraft:stained_hardened_clay");
    MU const Item LIGHT_GRAY_HARDENED_CLAY = Item(159, 8, "Light Gray Hardened Clay", "minecraft:stained_hardened_clay");
    MU const Item CYAN_HARDENED_CLAY = Item(159, 9, "Cyan Hardened Clay", "minecraft:stained_hardened_clay");
    MU const Item PURPLE_HARDENED_CLAY = Item(159, 10, "Purple Hardened Clay", "minecraft:stained_hardened_clay");
    MU const Item BLUE_HARDENED_CLAY = Item(159, 11, "Blue Hardened Clay", "minecraft:stained_hardened_clay");
    MU const Item BROWN_HARDENED_CLAY = Item(159, 12, "Brown Hardened Clay", "minecraft:stained_hardened_clay");
    MU const Item GREEN_HARDENED_CLAY = Item(159, 13, "Green Hardened Clay", "minecraft:stained_hardened_clay");
    MU const Item RED_HARDENED_CLAY = Item(159, 14, "Red Hardened Clay", "minecraft:stained_hardened_clay");
    MU const Item BLACK_HARDENED_CLAY = Item(159, 15, "Black Hardened Clay", "minecraft:stained_hardened_clay");

    MU const Item WHITE_STAINED_GLASS_PANE = Item(160, 0, "White Stained Glass Pane", "minecraft:stained_glass_pane");
    MU const Item ORANGE_STAINED_GLASS_PANE = Item(160, 1, "Orange Stained Glass Pane", "minecraft:stained_glass_pane");
    MU const Item MAGENTA_STAINED_GLASS_PANE = Item(160, 2, "Magenta Stained Glass Pane", "minecraft:stained_glass_pane");
    MU const Item LIGHT_BLUE_STAINED_GLASS_PANE = Item(160, 3, "Light Blue Stained Glass Pane", "minecraft:stained_glass_pane");
    MU const Item YELLOW_STAINED_GLASS_PANE = Item(160, 4, "Yellow Stained Glass Pane", "minecraft:stained_glass_pane");
    MU const Item LIME_STAINED_GLASS_PANE = Item(160, 5, "Lime Stained Glass Pane", "minecraft:stained_glass_pane");
    MU const Item PINK_STAINED_GLASS_PANE = Item(160, 6, "Pink Stained Glass Pane", "minecraft:stained_glass_pane");
    MU const Item GRAY_STAINED_GLASS_PANE = Item(160, 7, "Gray Stained Glass Pane", "minecraft:stained_glass_pane");
    MU const Item LIGHT_GRAY_STAINED_GLASS_PANE = Item(160, 8, "Light Gray Stained Glass Pane", "minecraft:stained_glass_pane");
    MU const Item CYAN_STAINED_GLASS_PANE = Item(160, 9, "Cyan Stained Glass Pane", "minecraft:stained_glass_pane");
    MU const Item PURPLE_STAINED_GLASS_PANE = Item(160, 10, "Purple Stained Glass Pane", "minecraft:stained_glass_pane");
    MU const Item BLUE_STAINED_GLASS_PANE = Item(160, 11, "Blue Stained Glass Pane", "minecraft:stained_glass_pane");
    MU const Item BROWN_STAINED_GLASS_PANE = Item(160, 12, "Brown Stained Glass Pane", "minecraft:stained_glass_pane");
    MU const Item GREEN_STAINED_GLASS_PANE = Item(160, 13, "Green Stained Glass Pane", "minecraft:stained_glass_pane");
    MU const Item RED_STAINED_GLASS_PANE = Item(160, 14, "Red Stained Glass Pane", "minecraft:stained_glass_pane");
    MU const Item BLACK_STAINED_GLASS_PANE = Item(160, 15, "Black Stained Glass Pane", "minecraft:stained_glass_pane");

    MU const Item ACACIA_LEAVES = Item(161, 0, "Acacia Leaves", "minecraft:leaves2");
    MU const Item DARK_OAK_LEAVES = Item(161, 1, "Dark Oak Leaves", "minecraft:leaves2");

    MU const Item ACACIA_WOOD = Item(162, 0, "Acacia Wood", "minecraft:log2");
    MU const Item DARK_OAK_WOOD = Item(162, 1, "Dark Oak Wood", "minecraft:log2");

    MU const Item ACACIA_WOOD_STAIRS = Item(163, 0, "Acacia Wood Stairs", "minecraft:acacia_stairs");
    MU const Item DARK_OAK_WOOD_STAIRS = Item(164, 0, "Dark Oak Wood Stairs", "minecraft:dark_oak_stairs");
    MU const Item SLIME_BLOCK = Item(165, 0, "Slime Block", "minecraft:slime");
    MU const Item BARRIER = Item(166, 0, "Barrier", "minecraft:barrier");
    MU const Item IRON_TRAPDOOR = Item(167, 0, "Iron Trapdoor", "minecraft:iron_trapdoor");

    MU const Item PRISMARINE = Item(168, 0, "Prismarine", "minecraft:prismarine");
    MU const Item PRISMARINE_BRICKS = Item(168, 1, "Prismarine Bricks", "minecraft:prismarine");
    MU const Item DARK_PRISMARINE = Item(168, 2, "Dark Prismarine", "minecraft:prismarine");

    MU const Item SEA_LANTERN = Item(169, 0, "Sea Lantern", "minecraft:sea_lantern");
    MU const Item HAY_BALE = Item(170, 0, "Hay Bale", "minecraft:hay_block");

    MU const Item WHITE_CARPET = Item(171, 0, "White White Carpet", "minecraft:carpet");
    MU const Item ORANGE_CARPET = Item(171, 1, "Orange White Carpet", "minecraft:carpet");
    MU const Item MAGENTA_CARPET = Item(171, 2, "Magenta White Carpet", "minecraft:carpet");
    MU const Item LIGHT_BLUE_CARPET = Item(171, 3, "Light Blue White Carpet", "minecraft:carpet");
    MU const Item YELLOW_CARPET = Item(171, 4, "Yellow White Carpet", "minecraft:carpet");
    MU const Item LIME_CARPET = Item(171, 5, "Lime White Carpet", "minecraft:carpet");
    MU const Item PINK_CARPET = Item(171, 6, "Pink White Carpet", "minecraft:carpet");
    MU const Item GRAY_CARPET = Item(171, 7, "Gray White Carpet", "minecraft:carpet");
    MU const Item LIGHT_GRAY_CARPET = Item(171, 8, "Light Gray White Carpet", "minecraft:carpet");
    MU const Item CYAN_CARPET = Item(171, 9, "Cyan White Carpet", "minecraft:carpet");
    MU const Item PURPLE_CARPET = Item(171, 10, "Purple White Carpet", "minecraft:carpet");
    MU const Item BLUE_CARPET = Item(171, 11, "Blue White Carpet", "minecraft:carpet");
    MU const Item BROWN_CARPET = Item(171, 12, "Brown White Carpet", "minecraft:carpet");
    MU const Item GREEN_CARPET = Item(171, 13, "Green White Carpet", "minecraft:carpet");
    MU const Item RED_CARPET = Item(171, 14, "Red White Carpet", "minecraft:carpet");
    MU const Item BLACK_CARPET = Item(171, 15, "Black White Carpet", "minecraft:carpet");

    MU const Item HARDENED_CLAY = Item(172, 0, "Hardened Clay", "minecraft:hardened_clay");
    MU const Item BLOCK_OF_COAL = Item(173, 0, "Block of Coal", "minecraft:coal_block");
    MU const Item PACKED_ICE = Item(174, 0, "Packed Ice", "minecraft:packed_ice");

    MU const Item SUNFLOWER = Item(175, 0, "Sunflower", "minecraft:double_plant");
    MU const Item LILAC = Item(175, 1, "Lilac", "minecraft:double_plant");
    MU const Item DOUBLE_TALLGRASS = Item(175, 2, "Double Tall Grass", "minecraft:double_plant");
    MU const Item LARGE_FERN = Item(175, 3, "Large Fern", "minecraft:double_plant");
    MU const Item ROSE_BUSH = Item(175, 4, "Rose Bush", "minecraft:double_plant");
    MU const Item PEONY = Item(175, 5, "Peony", "minecraft:double_plant");

    MU const Item FREE_STANDING_BANNER = Item(176, 0, "Free-standing Banner", "minecraft:standing_banner");

    MU const Item WALL_MOUNTED_BANNER = Item(177, 0, "Wall-mounted Banner", "minecraft:wall_banner");

    MU const Item INVERTED_DAYLIGHT_SENSOR = Item(178, 0, "Inverted Daylight Sensor", "minecraft:daylight_detector_inverted");

    MU const Item RED_SANDSTONE = Item(179, 0, "Red Sandstone", "minecraft:red_sandstone");
    MU const Item CHISELED_RED_SANDSTONE = Item(179, 1, "Chiseled Red Sandstone", "minecraft:red_sandstone");
    MU const Item SMOOTH_RED_SANDSTONE = Item(179, 2, "Smooth Red Sandstone", "minecraft:red_sandstone");

    MU const Item RED_SANDSTONE_STAIRS = Item(180, 0, "Red Sandstone Stairs", "minecraft:red_sandstone_stairs");

    MU const Item DOUBLE_RED_SANDSTONE_SLAB = Item(181, 0, "Double Red Sandstone Slab", "minecraft:double_stone_slab2");
    MU const Item SMOOTH_DOUBLE_RED_SANDSTONE_SLAB = Item(181, 8, "Smooth Double Red Sandstone Slab", "minecraft:double_stone_slab2");

    MU const Item RED_SANDSTONE_SLAB = Item(182, 0, "Red Sandstone Slab", "minecraft:stone_slab2");
    MU const Item UPPER_RED_SANDSTONE_SLAB = Item(182, 1, "Upper Red Sandstone Slab", "minecraft:stone_slab2");

    MU const Item SPRUCE_FENCE_GATE = Item(183, 0, "Spruce Fence Gate", "minecraft:spruce_fence_gate");
    MU const Item BIRCH_FENCE_GATE = Item(184, 0, "Birch Fence Gate", "minecraft:birch_fence_gate");
    MU const Item JUNGLE_FENCE_GATE = Item(185, 0, "Jungle Fence Gate", "minecraft:jungle_fence_gate");
    MU const Item DARK_OAK_FENCE_GATE = Item(186, 0, "Dark Oak Fence Gate", "minecraft:dark_oak_fence_gate");
    MU const Item ACACIA_FENCE_GATE = Item(187, 0, "Acacia Fence Gate", "minecraft:acacia_fence_gate");
    MU const Item SPRUCE_FENCE = Item(188, 0, "Spruce Fence", "minecraft:spruce_fence");
    MU const Item BIRCH_FENCE = Item(189, 0, "Birch Fence", "minecraft:birch_fence");
    MU const Item JUNGLE_FENCE = Item(190, 0, "Jungle Fence", "minecraft:jungle_fence");
    MU const Item DARK_OAK_FENCE = Item(191, 0, "Dark Oak Fence", "minecraft:dark_oak_fence");
    MU const Item ACACIA_FENCE = Item(192, 0, "Acacia Fence", "minecraft:acacia_fence");
    MU const Item SPRUCE_DOOR_BLOCK = Item(193, 0, "Spruce Door Block", "minecraft:spruce_door");
    MU const Item BIRCH_DOOR_BLOCK = Item(194, 0, "Birch Door Block", "minecraft:birch_door");
    MU const Item JUNGLE_DOOR_BLOCK = Item(195, 0, "Jungle Door Block", "minecraft:jungle_door");
    MU const Item ACACIA_DOOR_BLOCK = Item(196, 0, "Acacia Door Block", "minecraft:acacia_door");
    MU const Item DARK_OAK_DOOR_BLOCK = Item(197, 0, "Dark Oak Door Block", "minecraft:dark_oak_door");
    MU const Item END_ROD = Item(198, 0, "End Rod", "minecraft:end_rod");
    MU const Item CHORUS_PLANT = Item(199, 0, "Chorus Plant", "minecraft:chorus_plant");
    MU const Item CHORUS_FLOWER = Item(200, 0, "Chorus Flower", "minecraft:chorus_flower");
    MU const Item PURPUR_BLOCK = Item(201, 0, "Purpur Block", "minecraft:purpur_block");
    MU const Item PURPUR_PILLAR = Item(202, 0, "Purpur Pillar", "minecraft:purpur_pillar");
    MU const Item PURPUR_STAIRS = Item(203, 0, "Purpur Stairs", "minecraft:purpur_stairs");
    MU const Item PURPUR_DOUBLE_SLAB = Item(204, 0, "Purpur Double Slab", "minecraft:purpur_double_slab");
    MU const Item PURPUR_SLAB = Item(205, 0, "Purpur Slab", "minecraft:purpur_slab");
    MU const Item END_STONE_BRICKS = Item(206, 0, "End Stone Bricks", "minecraft:end_bricks");
    MU const Item BEETROOT_BLOCK = Item(207, 0, "Beetroot Block", "minecraft:beetroots");
    MU const Item GRASS_PATH = Item(208, 0, "Grass Path", "minecraft:grass_path");
    MU const Item END_GATEWAY = Item(209, 0, "End Gateway", "minecraft:end_gateway");
    MU const Item REPEATING_COMMAND_BLOCK = Item(210, 0, "Repeating Command Block", "minecraft:repeating_command_block");
    MU const Item CHAIN_COMMAND_BLOCK = Item(211, 0, "Chain Command Block", "minecraft:chain_command_block");
    MU const Item FROSTED_ICE = Item(212, 0, "Frosted Ice", "minecraft:frosted_ice");
    MU const Item MAGMA_BLOCK = Item(213, 0, "Magma Block", "minecraft:magma");
    MU const Item NETHER_WART_BLOCK = Item(214, 0, "Nether Wart Block", "minecraft:nether_wart_block");
    MU const Item RED_NETHER_BRICK = Item(215, 0, "Red Nether Brick", "minecraft:red_nether_brick");
    MU const Item BONE_BLOCK = Item(216, 0, "Bone Block", "minecraft:bone_block");
    MU const Item STRUCTURE_VOID = Item(217, 0, "Structure Void", "minecraft:structure_void");
    MU const Item OBSERVER = Item(218, 0, "Observer", "minecraft:observer");
    MU const Item WHITE_SHULKER_BOX = Item(219, 0, "White Shulker Box", "minecraft:white_shulker_box");
    MU const Item ORANGE_SHULKER_BOX = Item(220, 0, "Orange Shulker Box", "minecraft:orange_shulker_box");
    MU const Item MAGENTA_SHULKER_BOX = Item(221, 0, "Magenta Shulker Box", "minecraft:magenta_shulker_box");
    MU const Item LIGHT_BLUE_SHULKER_BOX = Item(222, 0, "Light Blue Shulker Box", "minecraft:light_blue_shulker_box");
    MU const Item YELLOW_SHULKER_BOX = Item(223, 0, "Yellow Shulker Box", "minecraft:yellow_shulker_box");
    MU const Item LIME_SHULKER_BOX = Item(224, 0, "Lime Shulker Box", "minecraft:lime_shulker_box");
    MU const Item PINK_SHULKER_BOX = Item(225, 0, "Pink Shulker Box", "minecraft:pink_shulker_box");
    MU const Item GRAY_SHULKER_BOX = Item(226, 0, "Gray Shulker Box", "minecraft:gray_shulker_box");
    MU const Item LIGHT_GRAY_SHULKER_BOX = Item(227, 0, "Light Gray Shulker Box", "minecraft:silver_shulker_box");
    MU const Item CYAN_SHULKER_BOX = Item(228, 0, "Cyan Shulker Box", "minecraft:cyan_shulker_box");
    MU const Item PURPLE_SHULKER_BOX = Item(229, 0, "Purple Shulker Box", "minecraft:purple_shulker_box");
    MU const Item BLUE_SHULKER_BOX = Item(230, 0, "Blue Shulker Box", "minecraft:blue_shulker_box");
    MU const Item BROWN_SHULKER_BOX = Item(231, 0, "Brown Shulker Box", "minecraft:brown_shulker_box");
    MU const Item GREEN_SHULKER_BOX = Item(232, 0, "Green Shulker Box", "minecraft:green_shulker_box");
    MU const Item RED_SHULKER_BOX = Item(233, 0, "Red Shulker Box", "minecraft:red_shulker_box");
    MU const Item BLACK_SHULKER_BOX = Item(234, 0, "Black Shulker Box", "minecraft:black_shulker_box");

    MU const Item WHITE_GLAZED_TERRACOTTA = Item(235, 0, "White Glazed Terracotta", "minecraft:white_glazed_terracotta");
    MU const Item ORANGE_GLAZED_TERRACOTTA = Item(236, 0, "Orange Glazed Terracotta", "minecraft:orange_glazed_terracotta");
    MU const Item MAGENTA_GLAZED_TERRACOTTA = Item(237, 0, "Magenta Glazed Terracotta", "minecraft:magenta_glazed_terracotta");
    MU const Item LIGHT_BLUE_GLAZED_TERRACOTTA = Item(238, 0, "Light Blue Glazed Terracotta", "minecraft:light_blue_glazed_terracotta");
    MU const Item YELLOW_GLAZED_TERRACOTTA = Item(239, 0, "Yellow Glazed Terracotta", "minecraft:yellow_glazed_terracotta");
    MU const Item LIME_GLAZED_TERRACOTTA = Item(240, 0, "Lime Glazed Terracotta", "minecraft:lime_glazed_terracotta");
    MU const Item PINK_GLAZED_TERRACOTTA = Item(241, 0, "Pink Glazed Terracotta", "minecraft:pink_glazed_terracotta");
    MU const Item GRAY_GLAZED_TERRACOTTA = Item(242, 0, "Gray Glazed Terracotta", "minecraft:gray_glazed_terracotta");
    MU const Item LIGHT_GRAY_GLAZED_TERRACOTTA = Item(243, 0, "Light Gray Glazed Terracotta", "minecraft:light_gray_glazed_terracotta");
    MU const Item CYAN_GLAZED_TERRACOTTA = Item(244, 0, "Cyan Glazed Terracotta", "minecraft:cyan_glazed_terracotta");
    MU const Item PURPLE_GLAZED_TERRACOTTA = Item(245, 0, "Purple Glazed Terracotta", "minecraft:purple_glazed_terracotta");
    MU const Item BLUE_GLAZED_TERRACOTTA = Item(246, 0, "Blue Glazed Terracotta", "minecraft:blue_glazed_terracotta");
    MU const Item BROWN_GLAZED_TERRACOTTA = Item(247, 0, "Brown Glazed Terracotta", "minecraft:brown_glazed_terracotta");
    MU const Item GREEN_GLAZED_TERRACOTTA = Item(248, 0, "Green Glazed Terracotta", "minecraft:green_glazed_terracotta");
    MU const Item RED_GLAZED_TERRACOTTA = Item(249, 0, "Red Glazed Terracotta", "minecraft:red_glazed_terracotta");
    MU const Item BLACK_GLAZED_TERRACOTTA = Item(250, 0, "Black Glazed Terracotta", "minecraft:black_glazed_terracotta");

    MU const Item WHITE_CONCRETE = Item(251, 0, "White Concrete", "minecraft:concrete");
    MU const Item ORANGE_CONCRETE = Item(251, 1, "Orange Concrete", "minecraft:concrete");
    MU const Item MAGENTA_CONCRETE = Item(251, 2, "Magenta Concrete", "minecraft:concrete");
    MU const Item LIGHT_BLUE_CONCRETE = Item(251, 3, "Light Blue Concrete", "minecraft:concrete");
    MU const Item YELLOW_CONCRETE = Item(251, 4, "Yellow Concrete", "minecraft:concrete");
    MU const Item LIME_CONCRETE = Item(251, 5, "Lime Concrete", "minecraft:concrete");
    MU const Item PINK_CONCRETE = Item(251, 6, "Pink Concrete", "minecraft:concrete");
    MU const Item GRAY_CONCRETE = Item(251, 7, "Gray Concrete", "minecraft:concrete");
    MU const Item LIGHT_GRAY_CONCRETE = Item(251, 8, "Light Gray Concrete", "minecraft:concrete");
    MU const Item CYAN_CONCRETE = Item(251, 9, "Cyan Concrete", "minecraft:concrete");
    MU const Item PURPLE_CONCRETE = Item(251, 10, "Purple Concrete", "minecraft:concrete");
    MU const Item BLUE_CONCRETE = Item(251, 11, "Blue Concrete", "minecraft:concrete");
    MU const Item BROWN_CONCRETE = Item(251, 12, "Brown Concrete", "minecraft:concrete");
    MU const Item GREEN_CONCRETE = Item(251, 13, "Green Concrete", "minecraft:concrete");
    MU const Item RED_CONCRETE = Item(251, 14, "Red Concrete", "minecraft:concrete");
    MU const Item BLACK_CONCRETE = Item(251, 15, "Black Concrete", "minecraft:concrete");

    MU const Item WHITE_CONCRETE_POWDER = Item(252, 0, "White Concrete Powder", "minecraft:concrete_powder");
    MU const Item ORANGE_CONCRETE_POWDER = Item(252, 1, "Orange Concrete Powder", "minecraft:concrete_powder");
    MU const Item MAGENTA_CONCRETE_POWDER = Item(252, 2, "Magenta Concrete Powder", "minecraft:concrete_powder");
    MU const Item LIGHT_BLUE_CONCRETE_POWDER = Item(252, 3, "Light Blue Concrete Powder", "minecraft:concrete_powder");
    MU const Item YELLOW_CONCRETE_POWDER = Item(252, 4, "Yellow Concrete Powder", "minecraft:concrete_powder");
    MU const Item LIME_CONCRETE_POWDER = Item(252, 5, "Lime Concrete Powder", "minecraft:concrete_powder");
    MU const Item PINK_CONCRETE_POWDER = Item(252, 6, "Pink Concrete Powder", "minecraft:concrete_powder");
    MU const Item GRAY_CONCRETE_POWDER = Item(252, 7, "Gray Concrete Powder", "minecraft:concrete_powder");
    MU const Item LIGHT_GRAY_CONCRETE_POWDER = Item(252, 8, "Light Gray Concrete Powder", "minecraft:concrete_powder");
    MU const Item CYAN_CONCRETE_POWDER = Item(252, 9, "Cyan Concrete Powder", "minecraft:concrete_powder");
    MU const Item PURPLE_CONCRETE_POWDER = Item(252, 10, "Purple Concrete Powder", "minecraft:concrete_powder");
    MU const Item BLUE_CONCRETE_POWDER = Item(252, 11, "Blue Concrete Powder", "minecraft:concrete_powder");
    MU const Item BROWN_CONCRETE_POWDER = Item(252, 12, "Brown Concrete Powder", "minecraft:concrete_powder");
    MU const Item GREEN_CONCRETE_POWDER = Item(252, 13, "Green Concrete Powder", "minecraft:concrete_powder");
    MU const Item RED_CONCRETE_POWDER = Item(252, 14, "Red Concrete Powder", "minecraft:concrete_powder");
    MU const Item BLACK_CONCRETE_POWDER = Item(252, 15, "Black Concrete Powder", "minecraft:concrete_powder");

    MU const Item STRUCTURE_BLOCK = Item(255, 0, "Structure Block", "minecraft:structure_block");

    MU const Item IRON_SHOVEL = Item(256, 0, ItemTool, "Iron Shovel", "minecraft:iron_shovel", true);
    MU const Item IRON_PICKAXE = Item(257, 0, ItemTool,"Iron Pickaxe", "minecraft:iron_pickaxe", true);
    MU const Item IRON_AXE = Item(258, 0, ItemTool,"Iron Axe", "minecraft:iron_axe", true);
    MU const Item FLINT_AND_STEEL = Item(259, 0, ItemTool, "Flint and Steel", "minecraft:flint_and_steel", true);
    MU const Item APPLE = Item(260, 0, "Apple", "minecraft:apple");
    MU const Item BOW = Item(261, 0, ItemBow, "Bow", "minecraft:bow");
    MU const Item ARROW = Item(262, 0, "Arrow", "minecraft:arrow");

    MU const Item COAL = Item(263, 0, "Coal", "minecraft:coal");
    MU const Item CHARCOAL = Item(263, 1, "Charcoal", "minecraft:coal");

    MU const Item DIAMOND = Item(264, 0, "Diamond", "minecraft:diamond");
    MU const Item IRON_INGOT = Item(265, 0, "Iron Ingot", "minecraft:iron_ingot");
    MU const Item GOLD_INGOT = Item(266, 0, "Gold Ingot", "minecraft:gold_ingot");

    MU const Item IRON_SWORD = Item(267, 0, ItemSword, "Iron Sword", "minecraft:iron_sword", true);

    MU const Sword WOODEN_SWORD = Sword(268, "Wooden Sword", "minecraft:wooden_sword", Tool::Materials::WOOD);
    MU const Tool WOODEN_SHOVEL = Tool(269, "Wooden Shovel", "minecraft:wooden_shovel", Tool::Materials::WOOD);
    MU const Tool WOODEN_PICKAXE = Tool(270, "Wooden Pickaxe", "minecraft:wooden_pickaxe", Tool::Materials::WOOD);
    MU const Tool WOODEN_AXE = Tool(271, "Wooden Axe", "minecraft:wooden_axe", Tool::Materials::WOOD);

    MU const Sword STONE_SWORD = Sword(272, "Stone Sword", "minecraft:stone_sword", Tool::Materials::STONE);
    MU const Tool STONE_SHOVEL = Tool(273, "Stone Shovel", "minecraft:stone_shovel", Tool::Materials::STONE);
    MU const Tool STONE_PICKAXE = Tool(274, "Stone Pickaxe", "minecraft:stone_pickaxe", Tool::Materials::STONE);
    MU const Tool STONE_AXE = Tool(275, "Stone Axe", "minecraft:stone_axe", Tool::Materials::STONE);

    MU const Sword DIAMOND_SWORD = Sword(276, "Diamond Sword", "minecraft:diamond_sword", Tool::Materials::DIAMOND);
    MU const Tool DIAMOND_SHOVEL = Tool(277, "Diamond Shovel", "minecraft:diamond_shovel", Tool::Materials::DIAMOND);
    MU const Tool DIAMOND_PICKAXE = Tool(278, "Diamond Pickaxe", "minecraft:diamond_pickaxe", Tool::Materials::DIAMOND);
    MU const Tool DIAMOND_AXE = Tool(279, "Diamond Axe", "minecraft:diamond_axe", Tool::Materials::DIAMOND);

    MU const Item STICK = Item(280, 0, "Stick", "minecraft:stick");
    MU const Item BOWL = Item(281, 0, "Bowl", "minecraft:bowl");
    MU const Item MUSHROOM_STEW = Item(282, 0, "Mushroom Stew", "minecraft:mushroom_stew");

    MU const Sword GOLDEN_SWORD = Sword(283, "Golden Sword", "minecraft:golden_sword", Tool::Materials::GOLD);
    MU const Tool GOLDEN_SHOVEL = Tool(284, "Golden Shovel", "minecraft:golden_shovel", Tool::Materials::GOLD);
    MU const Tool GOLDEN_PICKAXE = Tool(285, "Golden Pickaxe", "minecraft:golden_pickaxe", Tool::Materials::GOLD);
    MU const Tool GOLDEN_AXE = Tool(286, "Golden Axe", "minecraft:golden_axe", Tool::Materials::GOLD);

    MU const Item STRING = Item(287, 0, "String", "minecraft:string");
    MU const Item FEATHER = Item(288, 0, "Feather", "minecraft:feather");
    MU const Item GUNPOWDER = Item(289, 0, "Gunpowder", "minecraft:gunpowder");

    MU const Tool WOODEN_HOE = Tool(290, "Wooden Hoe", "minecraft:wooden_hoe", Tool::Materials::WOOD);
    MU const Tool STONE_HOE = Tool(291, "Stone Hoe", "minecraft:stone_hoe", Tool::Materials::STONE);
    MU const Tool IRON_HOE = Tool(292, "Iron Hoe", "minecraft:iron_hoe", Tool::Materials::IRON);
    MU const Tool DIAMOND_HOE = Tool(293, "Diamond Hoe", "minecraft:diamond_hoe", Tool::Materials::DIAMOND);
    MU const Tool GOLDEN_HOE = Tool(294, "Golden Hoe", "minecraft:golden_hoe", Tool::Materials::GOLD);

    MU const Item WHEAT_SEEDS = Item(295, 0, "Wheat Seeds", "minecraft:wheat_seeds");
    MU const Item WHEAT = Item(296, 0, "Wheat", "minecraft:wheat");
    MU const Item BREAD = Item(297, 0, "Bread", "minecraft:bread");

    MU const Armor LEATHER_HELMET = Armor(298, "Leather Helmet", "minecraft:leather_helmet", EntityEquipmentSlot::HEAD, Armor::Materials::LEATHER);
    MU const Armor LEATHER_TUNIC = Armor(299, "Leather Tunic", "minecraft:leather_chestplate", EntityEquipmentSlot::CHEST, Armor::Materials::LEATHER);
    MU const Armor LEATHER_PANTS = Armor(300, "Leather Pants", "minecraft:leather_leggings", EntityEquipmentSlot::LEGS, Armor::Materials::LEATHER);
    MU const Armor LEATHER_BOOTS = Armor(301, "Leather Boots", "minecraft:leather_boots", EntityEquipmentSlot::FEET, Armor::Materials::LEATHER);

    MU const Armor CHAINMAIL_HELMET = Armor(302, "Chainmail Helmet", "minecraft:chainmail_helmet", EntityEquipmentSlot::HEAD, Armor::Materials::CHAIN);
    MU const Armor CHAINMAIL_CHESTPLATE = Armor(303, "Chainmail Chestplate", "minecraft:chainmail_chestplate", EntityEquipmentSlot::CHEST, Armor::Materials::CHAIN);
    MU const Armor CHAINMAIL_LEGGINGS = Armor(304, "Chainmail Leggings", "minecraft:chainmail_leggings", EntityEquipmentSlot::LEGS, Armor::Materials::CHAIN);
    MU const Armor CHAINMAIL_BOOTS = Armor(305, "Chainmail Boots", "minecraft:chainmail_boots", EntityEquipmentSlot::FEET, Armor::Materials::CHAIN);

    MU const Armor IRON_HELMET = Armor(306, "Iron Helmet", "minecraft:iron_helmet", EntityEquipmentSlot::HEAD, Armor::Materials::IRON);
    MU const Armor IRON_CHESTPLATE = Armor(307, "Iron Chestplate", "minecraft:iron_chestplate", EntityEquipmentSlot::CHEST, Armor::Materials::IRON);
    MU const Armor IRON_LEGGINGS = Armor(308, "Iron Leggings", "minecraft:iron_leggings", EntityEquipmentSlot::LEGS, Armor::Materials::IRON);
    MU const Armor IRON_BOOTS = Armor(309, "Iron Boots", "minecraft:iron_boots", EntityEquipmentSlot::FEET, Armor::Materials::IRON);

    MU const Armor DIAMOND_HELMET = Armor(310, "Diamond Helmet", "minecraft:diamond_helmet", EntityEquipmentSlot::HEAD, Armor::Materials::DIAMOND);
    MU const Armor DIAMOND_CHESTPLATE = Armor(311, "Diamond Chestplate", "minecraft:diamond_chestplate", EntityEquipmentSlot::CHEST, Armor::Materials::DIAMOND);
    MU const Armor DIAMOND_LEGGINGS = Armor(312, "Diamond Leggings", "minecraft:diamond_leggings", EntityEquipmentSlot::LEGS, Armor::Materials::DIAMOND);
    MU const Armor DIAMOND_BOOTS = Armor(313, "Diamond Boots", "minecraft:diamond_boots", EntityEquipmentSlot::FEET, Armor::Materials::DIAMOND);

    MU const Armor GOLDEN_HELMET = Armor(314, "Golden Helmet", "minecraft:golden_helmet", EntityEquipmentSlot::HEAD, Armor::Materials::GOLD);
    MU const Armor GOLDEN_CHESTPLATE = Armor(315, "Golden Chestplate", "minecraft:golden_chestplate", EntityEquipmentSlot::CHEST, Armor::Materials::GOLD);
    MU const Armor GOLDEN_LEGGINGS = Armor(316, "Golden Leggings", "minecraft:golden_leggings", EntityEquipmentSlot::LEGS, Armor::Materials::GOLD);
    MU const Armor GOLDEN_BOOTS = Armor(317, "Golden Boots", "minecraft:golden_boots", EntityEquipmentSlot::FEET, Armor::Materials::GOLD);

    MU const Item FLINT = Item(318, 0, "Flint", "minecraft:flint");
    MU const Item RAW_PORKCHOP = Item(319, 0, "Raw Porkchop", "minecraft:porkchop");
    MU const Item COOKED_PORKCHOP = Item(320, 0, "Cooked Porkchop", "minecraft:cooked_porkchop");
    MU const Item PAINTING = Item(321, 0, "Painting", "minecraft:painting");

    MU const Item GOLDEN_APPLE = Item(322, 0, "Golden Apple", "minecraft:golden_apple");
    MU const Item ENCHANTED_GOLDEN_APPLE = Item(322, 1, "Enchanted Golden Apple", "minecraft:golden_apple");

    MU const Item SIGN = Item(323, 0, "Sign", "minecraft:sign");
    MU const Item OAK_DOOR = Item(324, 0, "Oak Door", "minecraft:wooden_door");
    MU const Item BUCKET = Item(325, 0, "Bucket", "minecraft:bucket");
    MU const Item WATER_BUCKET = Item(326, 0, "Water Bucket", "minecraft:water_bucket");
    MU const Item LAVA_BUCKET = Item(327, 0, "Lava Bucket", "minecraft:lava_bucket");
    MU const Item MINECART = Item(328, 0, "Minecart", "minecraft:minecart");
    MU const Item SADDLE = Item(329, 0, "Saddle", "minecraft:saddle");
    MU const Item IRON_DOOR = Item(330, 0, "Iron Door", "minecraft:iron_door");
    MU const Item REDSTONE = Item(331, 0, "Redstone", "minecraft:redstone");
    MU const Item SNOWBALL = Item(332, 0, "Snowball", "minecraft:snowball");
    MU const Item OAK_BOAT = Item(333, 0, "Oak Boat", "minecraft:boat");
    MU const Item LEATHER = Item(334, 0, "Leather", "minecraft:leather");
    MU const Item MILK_BUCKET = Item(335, 0, "Milk Bucket", "minecraft:milk_bucket");
    MU const Item BRICK = Item(336, 0, "Brick", "minecraft:brick");
    MU const Item CLAY = Item(337, 0, "Clay", "minecraft:clay_ball");
    MU const Item SUGAR_CANES_2 = Item(338, 0, "Sugar Canes", "minecraft:reeds");
    MU const Item PAPER = Item(339, 0, "Paper", "minecraft:paper");
    MU const Item BOOK = Item(340, 0, "Book", "minecraft:book");
    MU const Item SLIMEBALL = Item(341, 0, "Slimeball", "minecraft:slime_ball");
    MU const Item MINECART_WITH_CHEST = Item(342, 0, "Minecart with Chest", "minecraft:chest_minecart");
    MU const Item MINECART_WITH_FURNACE = Item(343, 0, "Minecart with Furnace", "minecraft:furnace_minecart");
    MU const Item EGG = Item(344, 0, "Egg", "minecraft:egg");
    MU const Item COMPASS = Item(345, 0, "Compass", "minecraft:compass");
    MU const Item FISHING_ROD = Item(346, 0, ItemFishingRod, "Fishing Rod", "minecraft:fishing_rod", true);
    MU const Item CLOCK = Item(347, 0, "Clock", "minecraft:clock");
    MU const Item GLOWSTONE_DUST = Item(348, 0, "Glowstone Dust", "minecraft:glowstone_dust");

    MU const Item RAW_FISH = Item(349, 0, "Raw Fish", "minecraft:fish");
    MU const Item RAW_SALMON = Item(349, 1, "Raw Fish", "minecraft:fish");
    MU const Item RAW_CLOWN_FISH = Item(349, 2, "Raw Fish", "minecraft:fish");
    MU const Item PUFFERFISH = Item(349, 3, "Pufferfish", "minecraft:fish");

    MU const Item COOKED_FISH = Item(350, 0, "Cooked Fish", "minecraft:cooked_fish");
    MU const Item COOKED_SALMON = Item(350, 1, "Cooked Salmon", "minecraft:cooked_fish");

    MU const Item INK_SACK = Item(351, 0, "Ink Sack", "minecraft:dye");
    MU const Item ROSE_RED = Item(351, 1, "Rose Red", "minecraft:dye");
    MU const Item CACTUS_GREEN = Item(351, 2, "Cactus Green", "minecraft:dye");
    MU const Item COCOA_BEANS = Item(351, 3, "Cocoa Beans", "minecraft:dye");
    MU const Item LAPIS_LAZULI = Item(351, 4, "Lapis Lazuli", "minecraft:dye");
    MU const Item PURPLE_DYE = Item(351, 5, "Purple Dye", "minecraft:dye");
    MU const Item CYAN_DYE = Item(351, 6, "Cyan Dye", "minecraft:dye");
    MU const Item LIGHT_GRAY_DYE = Item(351, 7, "Light Gray Dye", "minecraft:dye");
    MU const Item GRAY_DYE = Item(351, 8, "Gray Dye", "minecraft:dye");
    MU const Item PINK_DYE = Item(351, 9, "Pink Dye", "minecraft:dye");
    MU const Item LIME_DYE = Item(351, 10, "Lime Dye", "minecraft:dye");
    MU const Item DANDELION_YELLOW = Item(351, 11, "Dandelion Yellow", "minecraft:dye");
    MU const Item LIGHT_BLUE_DYE = Item(351, 12, "Light Blue Dye", "minecraft:dye");
    MU const Item MAGENTA_DYE = Item(351, 13, "Magenta Dye", "minecraft:dye");
    MU const Item ORANGE_DYE = Item(351, 14, "Orange Dye", "minecraft:dye");
    MU const Item BONE_MEAL = Item(351, 15, "Bone Meal", "minecraft:dye");

    MU const Item BONE = Item(352, 0, "Bone", "minecraft:bone");
    MU const Item SUGAR = Item(353, 0, "Sugar", "minecraft:sugar");
    MU const Item CAKE = Item(354, 0, "Cake", "minecraft:cake");

    MU const Item WHITE_BED = Item(355, 0, "White Bed", "minecraft:bed");
    MU const Item ORANGE_BED = Item(355, 1, "Orange Bed", "minecraft:bed");
    MU const Item MAGENTA_BED = Item(355, 2, "Magenta Bed", "minecraft:bed");
    MU const Item LIGHT_BLUE_BED = Item(355, 3, "Light Blue Bed", "minecraft:bed");
    MU const Item YELLOW_BED = Item(355, 4, "Yellow Bed", "minecraft:bed");
    MU const Item LIME_BED = Item(355, 5, "Lime Bed", "minecraft:bed");
    MU const Item PINK_BED = Item(355, 6, "Pink Bed", "minecraft:bed");
    MU const Item GRAY_BED = Item(355, 7, "Gray Bed", "minecraft:bed");
    MU const Item LIGHT_GRAY_BED = Item(355, 8, "Light Gray Bed", "minecraft:bed");
    MU const Item CYAN_BED = Item(355, 9, "Cyan Bed", "minecraft:bed");
    MU const Item PURPLE_BED = Item(355, 10, "Purple Bed", "minecraft:bed");
    MU const Item BLUE_BED = Item(355, 11, "Blue Bed", "minecraft:bed");
    MU const Item BROWN_BED = Item(355, 12, "Brown Bed", "minecraft:bed");
    MU const Item GREEN_BED = Item(355, 13, "Green Bed", "minecraft:bed");
    MU const Item RED_BED = Item(355, 14, "Red Bed", "minecraft:bed");
    MU const Item BLACK_BED = Item(355, 15, "Black Bed", "minecraft:bed");

    MU const Item REDSTONE_REPEATER = Item(356, 0, "Redstone Repeater", "minecraft:repeater");
    MU const Item COOKIE = Item(357, 0, "Cookie", "minecraft:cookie");
    MU const Item MAP = Item(358, 0, "Map", "minecraft:filled_map");
    MU const Item SHEARS = Item(359, 0, ItemTool, "Shears", "minecraft:shears", true);
    MU const Item MELON = Item(360, 0, "Melon", "minecraft:melon");
    MU const Item PUMPKIN_SEEDS = Item(361, 0, "Pumpkin Seeds", "minecraft:pumpkin_seeds");
    MU const Item MELON_SEEDS = Item(362, 0, "Melon Seeds", "minecraft:melon_seeds");
    MU const Item RAW_BEEF = Item(363, 0, "Raw Beef", "minecraft:beef");
    MU const Item STEAK = Item(364, 0, "Steak", "minecraft:cooked_beef");
    MU const Item RAW_CHICKEN = Item(365, 0, "Raw Chicken", "minecraft:chicken");
    MU const Item COOKED_CHICKEN = Item(366, 0, "Cooked Chicken", "minecraft:cooked_chicken");
    MU const Item ROTTEN_FLESH = Item(367, 0, "Rotten Flesh", "minecraft:rotten_flesh");
    MU const Item ENDER_PEARL = Item(368, 0, "Ender Pearl", "minecraft:ender_pearl");
    MU const Item BLAZE_ROD = Item(369, 0, "Blaze Rod", "minecraft:blaze_rod");
    MU const Item GHAST_TEAR = Item(370, 0, "Ghast Tear", "minecraft:ghast_tear");
    MU const Item GOLD_NUGGET = Item(371, 0, "Gold Nugget", "minecraft:gold_nugget");

    // Item NETHER_WART = Item(372, "Nether Wart", "minecraft:nether_wart");

    MU const Item POTION = Item(373, 0, "Potion", "minecraft:potion");
    MU const Item POTION_AWKWARD = Item(373, 0, "Awkward", "minecraft:potion");
    MU const Item MUNDANE_POTION = Item(373, 0, "Mundane Potion", "minecraft:potion");
    MU const Item POTION_OF_EXTENDED_INVISIBILITY = Item(373, 0, "Potion of Extended Invisibility", "minecraft:potion");
    MU const Item POTION_OF_EXTENDED_LEAPING = Item(373, 0, "Potion of Extended Leaping", "minecraft:potion");
    MU const Item POTION_OF_EXTENDED_NIGHT_VISION = Item(373, 0, "Potion of Extended Night Vision", "minecraft:potion");
    MU const Item POTION_OF_EXTENDED_POISON = Item(373, 0, "Potion of Extended Poison", "minecraft:potion");
    MU const Item POTION_OF_EXTENDED_REGENERATION = Item(373, 0, "Potion of Extended Regeneration", "minecraft:potion");
    MU const Item POTION_OF_EXTENDED_SLOWNESS = Item(373, 0, "Potion of Extended Slowness", "minecraft:potion");
    MU const Item POTION_OF_EXTENDED_STRENGTH = Item(373, 0, "Potion of Extended Strength", "minecraft:potion");
    MU const Item POTION_OF_EXTENDED_SWIFTNESS = Item(373, 0, "Potion of Extended Swiftness", "minecraft:potion");
    MU const Item POTION_OF_EXTENDED_WATER_BREATHING = Item(373, 0, "Potion of Extended Water Breathing", "minecraft:potion");
    MU const Item POTION_OF_EXTENDED_WEAKNESS = Item(373, 0, "Potion of Extended Weakness", "minecraft:potion");
    MU const Item POTION_OF_FIRE_RESISTANCE = Item(373, 0, "Potion of Fire Resistance", "minecraft:potion");
    MU const Item POTION_OF_HARMING = Item(373, 0, "Potion of Harming", "minecraft:potion");
    MU const Item POTION_OF_HARMING_II = Item(373, 0, "Potion of Harming II", "minecraft:potion");
    MU const Item POTION_OF_HEALING = Item(373, 0, "Potion of Healing", "minecraft:potion");
    MU const Item POTION_OF_HEALING_II = Item(373, 0, "Potion of Healing II", "minecraft:potion");
    MU const Item POTION_OF_INVISIBILITY = Item(373, 0, "Potion of Invisibility", "minecraft:potion");
    MU const Item POTION_OF_LEAPING = Item(373, 0, "Potion of Leaping", "minecraft:potion");
    MU const Item POTION_OF_LEAPING_II = Item(373, 0, "Potion of Leaping II", "minecraft:potion");
    MU const Item POTION_OF_LUCK = Item(373, 0, "Potion of Luck", "minecraft:potion");
    MU const Item POTION_OF_NIGHT_VISION = Item(373, 0, "Potion of Night Vision", "minecraft:potion");
    MU const Item POTION_OF_POISON = Item(373, 0, "Potion of Poison", "minecraft:potion");
    MU const Item POTION_OF_POISON_II = Item(373, 0, "Potion of Poison II", "minecraft:potion");
    MU const Item POTION_OF_REGENERATION = Item(373, 0, "Potion of Regeneration", "minecraft:potion");
    MU const Item POTION_OF_REGENERATION_II = Item(373, 0, "Potion of Regeneration II", "minecraft:potion");
    MU const Item POTION_OF_SLOWNESS = Item(373, 0, "Potion of Slowness", "minecraft:potion");
    MU const Item POTION_OF_STRENGTH = Item(373, 0, "Potion of Strength", "minecraft:potion");
    MU const Item POTION_OF_STRENGTH_II = Item(373, 0, "Potion of Strength II", "minecraft:potion");
    MU const Item POTION_OF_SWIFTNESS = Item(373, 0, "Potion of Swiftness", "minecraft:potion");
    MU const Item POTION_OF_SWIFTNESS_II = Item(373, 0, "Potion of Swiftness II", "minecraft:potion");
    MU const Item POTION_OF_WATER_BREATHING = Item(373, 0, "Potion of Water Breathing", "minecraft:potion");
    MU const Item POTION_OF_WEAKNESS = Item(373, 0, "Potion of Weakness", "minecraft:potion");
    MU const Item THICK_POTION = Item(373, 0, "Thick Potion", "minecraft:potion");
    MU const Item UNCRAFTABLE_POTION = Item(373, 0, "Uncraftable Potion", "minecraft:potion");
    MU const Item WATER_BOTTLE = Item(373, 0, "Water Bottle", "minecraft:potion");

    MU const Item GLASS_BOTTLE = Item(374, 0, "Glass Bottle", "minecraft:glass_bottle");
    MU const Item SPIDER_EYE = Item(375, 0, "Spider Eye", "minecraft:spider_eye");
    MU const Item FERMENTED_SPIDER_EYE = Item(376, 0, "Fermented Spider Eye", "minecraft:fermented_spider_eye");
    MU const Item BLAZE_POWDER = Item(377, 0, "Blaze Powder", "minecraft:blaze_powder");
    MU const Item MAGMA_CREAM = Item(378, 0, "Magma Cream", "minecraft:magma_cream");
    // Item BREWING_STAND = Item(379, 0, "Brewing Stand", "minecraft:brewing_stand");
    // Item CAULDRON = Item(380, 0, "Cauldron", "minecraft:cauldron");
    MU const Item EYE_OF_ENDER = Item(381, 0, "Eye of Ender", "minecraft:ender_eye");
    MU const Item GLISTERING_MELON = Item(382, 0, "Glistering Melon", "minecraft:speckled_melon");

    MU const Item SPAWN_EGG = Item(383, 0, "Spawn Egg", "");
    // times 1000 million

    MU const Item BOTTLE_O_ENCHANTING = Item(384, 0, "Bottle o' Enchanting", "minecraft:experience_bottle");
    MU const Item FIRE_CHARGE = Item(385, 0, "Fire Charge", "minecraft:fire_charge");
    MU const Item BOOK_AND_QUILL = Item(386, 0, "Book and Quill", "minecraft:writable_book");
    MU const Item WRITTEN_BOOK = Item(387, 0, "Written Book", "minecraft:written_book");
    MU const Item EMERALD = Item(388, 0, "Emerald", "minecraft:emerald");
    MU const Item ITEM_FRAME = Item(389, 0, "Item Frame", "minecraft:item_frame");
    // Item FLOWER_POT = Item(390, 0, "Flower Pot", "minecraft:flower_pot");
    MU const Item CARROT = Item(391, 0, "Carrot", "minecraft:carrot");
    MU const Item POTATO = Item(392, 0, "Potato", "minecraft:potato");
    MU const Item BAKED_POTATO = Item(393, 0, "Baked Potato", "minecraft:baked_potato");
    MU const Item POISONOUS_POTATO = Item(394, 0, "Poisonous Potato", "minecraft:poisonous_potato");
    MU const Item EMPTY_MAP = Item(395, 0, "Empty Map", "minecraft:map");
    MU const Item GOLDEN_CARROT = Item(396, 0, "Golden Carrot", "minecraft:golden_carrot");

    MU const Item SKELETON_SKULL = Item(397, 0, ItemSkull, "Skeleton Skull", "minecraft:skull");
    MU const Item WITHER_SKELETON_SKULL = Item(397, 1, ItemSkull, "Wither Skeleton Skull", "minecraft:skull");
    MU const Item ZOMBIE_HEAD = Item(397, 2, ItemSkull, "Zombie Head", "minecraft:skull");
    MU const Item PLAYER_HEAD = Item(397, 3, ItemSkull, "Player Head", "minecraft:skull");
    MU const Item CREEPER_HEAD = Item(397, 4, ItemSkull, "Creeper Head", "minecraft:skull");
    MU const Item DRAGON_HEAD = Item(397, 5, ItemSkull, "Dragon Head", "minecraft:skull");

    MU const Item CARROT_ON_A_STICK = Item(398, 0, "Carrot on a Stick", "minecraft:carrot_on_a_stick");
    MU const Item NETHER_STAR = Item(399, 0, "Nether Star", "minecraft:nether_star");
    MU const Item PUMPKIN_PIE = Item(400, 0, "Pumpkin Pie", "minecraft:pumpkin_pie");
    MU const Item FIREWORK_ROCKET = Item(401, 0, "Firework Rocket", "minecraft:fireworks");
    MU const Item FIREWORK_STAR = Item(402, 0, "Firework Star", "minecraft:firework_charge");
    MU const Item ENCHANTED_BOOK = Item(403, 0, "Enchanted Book", "minecraft:enchanted_book");
    MU const Item REDSTONE_COMPARATOR = Item(404, 0, "Redstone Comparator", "minecraft:comparator");
    MU const Item NETHER_BRICK = Item(405, 0, "Nether Brick", "minecraft:netherbrick");
    MU const Item NETHER_QUARTZ = Item(406, 0, "Nether Quartz", "minecraft:quartz");
    MU const Item MINECART_WITH_TNT = Item(407, 0, "Minecart with TNT", "minecraft:tnt_minecart");
    MU const Item MINECART_WITH_HOPPER = Item(408, 0, "Minecart with Hopper", "minecraft:hopper_minecart");
    MU const Item PRISMARINE_SHARD = Item(409, 0, "Prismarine Shard", "minecraft:prismarine_shard");
    MU const Item PRISMARINE_CRYSTALS = Item(410, 0, "Prismarine Crystals", "minecraft:prismarine_crystals");
    MU const Item RAW_RABBIT = Item(411, 0, "Raw Rabbit", "minecraft:rabbit");
    MU const Item COOKED_RABBIT = Item(412, 0, "Cooked Rabbit", "minecraft:cooked_rabbit");
    MU const Item RABBIT_STEW = Item(413, 0, "Rabbit Stew", "minecraft:rabbit_stew");
    MU const Item RABBITS_FOOT = Item(414, 0, "Rabbit’s Foot", "minecraft:rabbit_foot");
    MU const Item RABBIT_HIDE = Item(415, 0, "Rabbit Hide", "minecraft:rabbit_hide");
    MU const Item ARMOR_STAND = Item(416, 0, "Armor Stand", "minecraft:armor_stand");
    MU const Item IRON_HORSE_ARMOR = Item(417, 0, "Iron Horse Armor", "minecraft:iron_horse_armor");
    MU const Item GOLDEN_HORSE_ARMOR = Item(418, 0, "Golden Horse Armor", "minecraft:golden_horse_armor");
    MU const Item DIAMOND_HORSE_ARMOR = Item(419, 0, "Diamond Horse Armor", "minecraft:diamond_horse_armor");
    MU const Item LEAD = Item(420, 0, "Lead", "minecraft:lead");
    MU const Item NAME_TAG = Item(421, 0, "Name Tag", "minecraft:name_tag");
    MU const Item MINECART_WITH_COMMAND_BLOCK = Item(422, 0, "Minecart with Command Block", "minecraft:command_block_minecart");
    MU const Item RAW_MUTTON = Item(423, 0, "Raw Mutton", "minecraft:mutton");
    MU const Item COOKED_MUTTON = Item(424, 0, "Cooked Mutton", "minecraft:cooked_mutton");

    MU const Item BLACK_BANNER = Item(425, 0, "Black Banner", "minecraft:banner");
    MU const Item RED_BANNER = Item(425, 1, "Red Banner", "minecraft:banner");
    MU const Item GREEN_BANNER = Item(425, 2, "Green Banner", "minecraft:banner");
    MU const Item BROWN_BANNER = Item(425, 3, "Brown Banner", "minecraft:banner");
    MU const Item BLUE_BANNER = Item(425, 4, "Blue Banner", "minecraft:banner");
    MU const Item PURPLE_BANNER = Item(425, 5, "Purple Banner", "minecraft:banner");
    MU const Item CYAN_BANNER = Item(425, 6, "Cyan Banner", "minecraft:banner");
    MU const Item LIGHT_BANNER = Item(425, 7, "Light Gray Banner", "minecraft:banner");
    MU const Item GRAY_BANNER = Item(425, 8, "Gray Banner", "minecraft:banner");
    MU const Item PINK_BANNER = Item(425, 9, "Pink Banner", "minecraft:banner");
    MU const Item LIME_BANNER = Item(425, 10, "Lime Banner", "minecraft:banner");
    MU const Item YELLOW_BANNER = Item(425, 11, "Yellow Banner", "minecraft:banner");
    MU const Item LIGHT_BLUE_BANNER = Item(425, 12, "Light Blue Banner", "minecraft:banner");
    MU const Item MAGENTA_BANNER = Item(425, 13, "Magenta Banner", "minecraft:banner");
    MU const Item ORANGE_BANNER = Item(425, 14, "Orange Banner", "minecraft:banner");
    MU const Item WHITE_BANNER = Item(425, 15, "White Banner", "minecraft:banner");

    MU const Item END_CRYSTAL = Item(426, 0, "End Crystal", "minecraft:end_crystal");
    MU const Item SPRUCE_DOOR = Item(427, 0, "Spruce Door", "minecraft:spruce_door");
    MU const Item BIRCH_DOOR = Item(428, 0, "Birch Door", "minecraft:birch_door");
    MU const Item JUNGLE_DOOR = Item(429, 0, "Jungle Door", "minecraft:jungle_door");
    MU const Item ACACIA_DOOR = Item(430, 0, "Acacia Door", "minecraft:acacia_door");
    MU const Item DARK_OAK_DOOR = Item(431, 0, "Dark Oak Door", "minecraft:dark_oak_door");
    MU const Item CHORUS_FRUIT = Item(432, 0, "Chorus Fruit", "minecraft:chorus_fruit");
    MU const Item POPPED_CHORUS_FRUIT = Item(433, 0, "Popped Chorus Fruit", "minecraft:popped_chorus_fruit");
    MU const Item BEETROOT = Item(434, 0, "Beetroot", "minecraft:beetroot");
    MU const Item BEETROOT_SEEDS = Item(435, 0, "Beetroot Seeds", "minecraft:beetroot_seeds");
    MU const Item BEETROOT_SOUP = Item(436, 0, "Beetroot Soup", "minecraft:beetroot_soup");
    MU const Item DRAGONS_BREATH = Item(437, 0, "Dragon’s Breath", "minecraft:dragon_breath");
    MU const Item SPLASH_POTION = Item(438, 0, "Splash Potion", "minecraft:splash_potion");
    MU const Item SPECTRAL_ARROW = Item(439, 0, "Spectral Arrow", "minecraft:spectral_arrow");
    MU const Item TIPPED_ARROW = Item(440, 0, "Tipped Arrow", "minecraft:tipped_arrow");
    MU const Item LINGERING_POTION = Item(441, 0, "Lingering Potion", "minecraft:lingering_potion");
    MU const Item SHIELD = Item(442, 0, "Shield", "minecraft:shield");
    MU const Item ELYTRA = Item(443, 0, ItemElytra, "Elytra", "minecraft:elytra");
    MU const Item SPRUCE_BOAT = Item(444, 0, "Spruce Boat", "minecraft:spruce_boat");
    MU const Item BIRCH_BOAT = Item(445, 0, "Birch Boat", "minecraft:birch_boat");
    MU const Item JUNGLE_BOAT = Item(446, 0, "Jungle Boat", "minecraft:jungle_boat");
    MU const Item ACACIA_BOAT = Item(447, 0, "Acacia Boat", "minecraft:acacia_boat");
    MU const Item DARK_OAK_BOAT = Item(448, 0, "Dark Oak Boat", "minecraft:dark_oak_boat");
    MU const Item TOTEM_OF_UNDYING = Item(449, 0, "Totem of Undying", "minecraft:totem_of_undying");
    MU const Item SHULKER_SHELL = Item(450, 0, "Shulker Shell", "minecraft:shulker_shell");
    MU const Item IRON_NUGGET = Item(452, 0, "Iron Nugget", "minecraft:iron_nugget");
    MU const Item KNOWLEDGE_BOOK = Item(453, 0, "Knowledge Book", "minecraft:knowledge_book");

    MU const Item HEART_OF_THE_SEA = Item(571, 0, "Heart of the Sea", "minecraft:heart_of_the_sea");

    MU const Item DISC_13 = Item(2256, 0, "13 Disc", "minecraft:record_13");
    MU const Item DISC_CAT = Item(2257, 0, "Cat Disc", "minecraft:record_cat");
    MU const Item DISC_BLOCKS = Item(2258, 0, "Blocks Disc", "minecraft:record_blocks");
    MU const Item DISC_CHIRP = Item(2259, 0, "Chirp Disc", "minecraft:record_chirp");
    MU const Item DISC_FAR = Item(2260, 0, "Far Disc", "minecraft:record_far");
    MU const Item DISC_MALL = Item(2261, 0, "Mall Disc", "minecraft:record_mall");
    MU const Item DISC_MELLOHI = Item(2262, 0, "Mellohi Disc", "minecraft:record_mellohi");
    MU const Item DISC_STAL = Item(2263, 0, "Stal Disc", "minecraft:record_stal");
    MU const Item DISC_STRAD = Item(2264, 0, "Strad Disc", "minecraft:record_strad");
    MU const Item WARD_DISC = Item(2265, 0, "Ward Disc", "minecraft:record_ward");
    MU const Item DISC_11 = Item(2266, 0, "11 Disc", "minecraft:record_11");
    MU const Item DISC_WAIT = Item(2267, 0, "Wait Disc", "minecraft:record_wait");

}
