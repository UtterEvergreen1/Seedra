#pragma once

#include "Item.hpp"

namespace Items {
    class Trident : public Item {
    public:
        Trident(int id, int dataTag, std::string itemName, std::string ident)
            : Item(id, dataTag, ItemTrident, std::move(itemName), std::move(ident)) {}
    };

} // namespace Items
