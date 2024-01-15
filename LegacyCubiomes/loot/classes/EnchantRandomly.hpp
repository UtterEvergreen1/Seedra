#pragma once

#include "LootFunction.hpp"


class EnchantRandomlyBook final : public LootFunction {
public:
    void apply(ItemStack& itemStack, RNG& random) override;
};


class EnchantRandomlyItem final : public LootFunction {
public:
    void apply(ItemStack& itemStack, RNG& random) override;
};