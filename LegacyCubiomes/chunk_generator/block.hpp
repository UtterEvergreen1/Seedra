#pragma once

#include "lce/items/items.hpp"

class Block {
public:
    u16 block;
    u8 data;

    Block(c_u16 blockIn, c_u8 dataIn) : block(blockIn), data(dataIn) {}

    explicit Block(c_u16 blockIn) : Block(blockIn, 0) {}

    Block() : Block(0, 0) {}

    explicit Block(const lce::items::Item& item) : Block(item.getID(), item.getDataTag()) {}
};
