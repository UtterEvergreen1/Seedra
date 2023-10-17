#include "EnchantRandomly.hpp"


void EnchantRandomlyBook::apply(ItemStack& itemStack, RNG& random) {
    Enchantment *enchPtr = Enchantment::REGISTRY[random.nextInt((int) Enchantment::REGISTRY.size())];
    int level = random.nextInt(1, enchPtr->maxLevel);
    itemStack.addEnchantment(enchPtr, level);
}


void EnchantRandomlyItem::apply(ItemStack& itemStack, RNG& random) {
    std::vector<Enchantment*> list;
    for (Enchantment* enchantmentPointer: Enchantment::REGISTRY.getRegistry())
        if (enchantmentPointer->canApply(itemStack.item))
            list.emplace_back(enchantmentPointer);

    Enchantment* enchPtr = list[random.nextInt((int) list.size())];
    int level = random.nextInt(1, enchPtr->maxLevel);
    itemStack.addEnchantment(enchPtr, level);
}