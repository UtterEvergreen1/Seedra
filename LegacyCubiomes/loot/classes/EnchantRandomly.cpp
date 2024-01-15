#include "EnchantRandomly.hpp"


void EnchantRandomlyBook::apply(ItemStack& itemStack, RNG& random) {
    Enchantment* enchPtr = Enchantment::REGISTRY[random.nextInt(Enchantment::REGISTRY.size())];
    const int level = random.nextInt(1, enchPtr->maxLevel);
    itemStack.addEnchantment(enchPtr, level);
}


void EnchantRandomlyItem::apply(ItemStack& itemStack, RNG& random) {
    std::vector<Enchantment*> list;
    for (Enchantment* enchantmentPointer: Enchantment::REGISTRY.getRegistry())
        if (enchantmentPointer->canApply(itemStack.item)) list.emplace_back(enchantmentPointer);

    Enchantment* enchPtr = list[random.nextInt(static_cast<int>(list.size()))];
    const int level = random.nextInt(1, enchPtr->maxLevel);
    itemStack.addEnchantment(enchPtr, level);
}