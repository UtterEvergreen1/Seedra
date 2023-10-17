#pragma once

#include "LootFunction.hpp"


class EnchantRandomlyBook : public LootFunction {
public:
    void apply(ItemStack& itemStack, RNG& random) final;
};

class EnchantRandomlyItem : public LootFunction {
public:
    void apply(ItemStack& itemStack, RNG& random) final;
};