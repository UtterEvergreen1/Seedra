#pragma once

#include "Item.hpp"


namespace Items {
    class Tool : public Item {
    public:
        Tool(const int id, std::string itemName, std::string ident, const int8_t materialEnchantability)
            : Item(id, 0, ItemType::ItemTool, std::move(itemName), std::move(ident), true, materialEnchantability) {}
    };

}
