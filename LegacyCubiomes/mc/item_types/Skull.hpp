#pragma once

#include "Item.hpp"


namespace Items {
    class Skull : public Item {
    public:
        Skull(const int id, const int dataTag, std::string itemName, std::string ident)
            : Item(id, dataTag, ItemType::ItemSkull, std::move(itemName), std::move(ident)) {}
    };

}
