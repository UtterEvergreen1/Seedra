#pragma once

#include "lce/items/items.hpp"

class Block {
public:
    uint16_t block;
    uint8_t data;

    Block(const uint16_t blockIn, const uint8_t dataIn) : block(blockIn), data(dataIn) {}

    explicit Block(const uint16_t blockIn) : Block(blockIn, 0) {}

    Block() : Block(0, 0) {}

    explicit Block(const lce::items::Item& item) : Block(item.getID(), item.getDataTag()) {}
};
