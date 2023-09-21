#pragma once


namespace Items {

    enum ItemType {
        ItemArmor,
        ItemBlock,
        ItemBow,
        ItemElytra,
        ItemFishingRod,
        ItemSkull,
        ItemSword,
        ItemTool,
        ItemTrident
    };

    struct EntityEquipmentSlot {
        static constexpr int NONE = -1;
        static constexpr int FEET = 0;
        static constexpr int LEGS = 1;
        static constexpr int CHEST = 2;
        static constexpr int HEAD = 3;
    };

    /** represents enchantability for a specific material */
    struct ArmorMaterials {
        static constexpr int LEATHER = 15;
        static constexpr int CHAIN = 12;
        static constexpr int IRON = 9;
        static constexpr int GOLD = 25;
        static constexpr int DIAMOND = 1;
    };

    /** represents enchantability for a specific material */
    struct ToolMaterials {
        static constexpr int WOOD = 15;
        static constexpr int STONE = 5;
        static constexpr int IRON = 14;
        static constexpr int DIAMOND = 10;
        static constexpr int GOLD = 22;
    };




}