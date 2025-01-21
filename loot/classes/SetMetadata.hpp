#pragma once

#include "LootFunction.hpp"
#include "UniformRoll.hpp"

class SetMetadata final : public LootFunction {
    UniformRoll metaRange;
    const std::vector<const lce::Item*> dataItems;

public:
    SetMetadata(UniformRoll range, const std::vector<const lce::Item*>& dataItemsIn);
    void apply(ItemStack& itemStack, RNG& random) override;
};
