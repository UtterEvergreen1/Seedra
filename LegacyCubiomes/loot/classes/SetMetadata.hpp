#pragma once
#include "LootFunction.hpp"
#include "UniformRoll.hpp"

class SetMetadata : public LootFunction {
private:
    UniformRoll metaRange;
    const std::vector<const Items::Item*> dataItems;

public:
    SetMetadata(UniformRoll range, const std::vector<const Items::Item*>& dataItemsIn);
    void apply(ItemStack& itemStack, RNG& random) final;
};
