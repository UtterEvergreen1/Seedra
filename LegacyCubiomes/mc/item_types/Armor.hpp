#pragma once

#include "Item.hpp"

namespace Items {
    class Armor : public Item {
    public:
        Armor(int id, const std::string& itemName, const std::string& ident, EntityEquipSlot slot,
              int8_t materialEnchantability)
            : Item(id, 0, ItemArmor, itemName, ident, slot, materialEnchantability) {}
    };

} // namespace Items
