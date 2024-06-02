#pragma once

#include <cstdint>


namespace lce::items {

    /// i forgot what this is used for
    enum class ItemType : int8_t {
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

    /// represents what armor slots are allowed for a specific item
    enum class EntityEquipSlot : int8_t {
        NONE = -1,
        FEET = 0,
        LEGS = 1,
        CHEST = 2,
        HEAD = 3,
    };

    /// represents enchantability for a specific material
    enum ArmorMaterials : int8_t {
        ARMOR_LEATHER = 15,
        ARMOR_CHAIN = 12,
        ARMOR_IRON = 9,
        ARMOR_GOLD = 25,
        ARMOR_DIAMOND = 1,
    };

    /// represents enchantability for a specific material
    enum ToolMaterials : int8_t {
        TOOL_WOOD = 15,
        TOOL_STONE = 5,
        TOOL_IRON = 14,
        TOOL_DIAMOND = 10,
        TOOL_GOLD = 22,
    };

}