#pragma once

#include "Item.hpp"


namespace Items {
    class Sword : public Item {
    public:
        Sword(const int id, std::string itemName, std::string ident, const ToolMaterials materialEnchantability)
            : Item(id, 0, ItemType::ItemSword, std::move(itemName), std::move(ident), true, materialEnchantability) {}
    };

}
