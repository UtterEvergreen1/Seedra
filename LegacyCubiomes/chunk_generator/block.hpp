#pragma once

#include <cmath>
#include <string>

#include "LegacyCubiomes/mc/items.hpp"

class Block {
public:
    uint16_t block;
    uint8_t data;

    Block(uint16_t blockIn, uint8_t dataIn) : block(blockIn), data(dataIn) {}

    explicit Block(uint16_t blockIn) : Block(blockIn, 0) {}

    Block() : Block(0, 0) {}

    explicit Block(const Items::Item &item) : Block(item.getID(), item.getDataTag()) {}
};
