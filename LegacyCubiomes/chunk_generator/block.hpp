#pragma once

#include <cmath>
#include <string>

#include "LegacyCubiomes/mc/items.hpp"

class Block
{
public:
    uint16_t block;
    uint8_t data;
    Block() : block(0), data(0) {}
    explicit Block(uint16_t blockIn) : block(blockIn), data(0) {}
    Block(uint16_t blockIn, uint8_t dataIn) : block(blockIn), data(dataIn) {}
    explicit Block(const Items::Item& item) : block(item.id), data(item.dataTag) {}
};
