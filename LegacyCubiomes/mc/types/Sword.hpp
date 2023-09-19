#pragma once

#include "Item.hpp"


namespace Items {

    class Sword : public Item {
    public:
        Sword(int id, std::string itemName, std::string ident, int materialEnchantability)
                : Item(id, 0, ItemSword, std::move(itemName), std::move(ident), true, materialEnchantability) {
        }
    };

}





