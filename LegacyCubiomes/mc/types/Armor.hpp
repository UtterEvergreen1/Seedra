#pragma once

#include "Item.hpp"

namespace Items {
    class Armor : public Item {
    public:
        Armor(int id, std::string itemName, std::string ident, int slot, int materialEnchantability)
                : Item(id, 0, ItemArmor, std::move(itemName), std::move(ident), slot, materialEnchantability) {
        }
    };

}




