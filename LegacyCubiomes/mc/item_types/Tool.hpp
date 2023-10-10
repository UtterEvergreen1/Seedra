#pragma once

#include "Item.hpp"


namespace Items {
    class Tool : public Item {
    public:
        Tool(int id, std::string itemName, std::string ident, int8_t materialEnchantability)
                : Item(id, 0, ItemTool, std::move(itemName), std::move(ident), true, materialEnchantability) {
        }
    };

}




