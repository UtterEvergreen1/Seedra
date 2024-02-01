#pragma once

#include "Item.hpp"


namespace Items {
    class Trident : public Item {
    public:
        Trident(const int id, const int dataTag, std::string itemName, std::string ident)
            : Item(id, dataTag, ItemType::ItemTrident, std::move(itemName), std::move(ident)) {}
    };

}
