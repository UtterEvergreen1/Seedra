#pragma once

#include "Item.hpp"


namespace Items {
    class Skull : public Item {
    public:
        Skull(int id, int dataTag, std::string itemName, std::string ident)
                : Item(id, dataTag, ItemSkull, std::move(itemName), std::move(ident)) {
        }
    };

}




