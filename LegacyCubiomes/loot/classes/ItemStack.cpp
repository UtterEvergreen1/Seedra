#include "ItemStack.hpp"


ItemStack ItemStack::splitStack(int amount) {
    int splitCount = std::min(amount, (int) stackSize);
    ItemStack splitItem(item, splitCount);
    stackSize -= splitCount;
    return splitItem;
}


ND const Items::Item* ItemStack::getItem() const { return item; }


void ItemStack::addEnchantment(Enchantment* enchantment, int level) { enchantments.emplace_back(enchantment, level); }


void ItemStack::addEnchantmentData(EnchantmentData* enchantmentData) { enchantments.emplace_back(*enchantmentData); }
