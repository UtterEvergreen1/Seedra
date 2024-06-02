#pragma once

#include "Item.hpp"


namespace lce::items {

    class Armor : public Item {
    public:
        Armor(const int id, const std::string& itemName, const std::string& ident, const EntityEquipSlot slot,
              const int8_t materialEnchantability)
            : Item(id, 0, ItemType::ItemArmor, itemName, ident, slot, materialEnchantability) {}
    };

    class Skull : public Item {
    public:
        Skull(const int id, const int dataTag, std::string itemName, std::string ident)
            : Item(id, dataTag, ItemType::ItemSkull, std::move(itemName), std::move(ident)) {}
    };

    class Sword : public Item {
    public:
        Sword(const int id, std::string itemName, std::string ident, const ToolMaterials materialEnchantability)
            : Item(id, 0, ItemType::ItemSword, std::move(itemName), std::move(ident), true, materialEnchantability) {}
    };

    class Tool : public Item {
    public:
        Tool(const int id, std::string itemName, std::string ident, const int8_t materialEnchantability)
            : Item(id, 0, ItemType::ItemTool, std::move(itemName), std::move(ident), true, materialEnchantability) {}
    };

    class Trident : public Item {
    public:
        Trident(const int id, const int dataTag, std::string itemName, std::string ident)
            : Item(id, dataTag, ItemType::ItemTrident, std::move(itemName), std::move(ident)) {}
    };

}