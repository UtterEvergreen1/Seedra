#pragma once

#include "Item.hpp"


namespace Items {
    class Armor : public Item {
    public:
        Armor(const int id, const std::string& itemName, const std::string& ident, const EntityEquipSlot slot,
              const int8_t materialEnchantability)
            : Item(id, 0, ItemType::ItemArmor, itemName, ident, slot, materialEnchantability) {}
    };

}
