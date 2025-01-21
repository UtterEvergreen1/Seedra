#include "ItemStack.hpp"


ItemStack ItemStack::splitStack(c_int amount) {
    c_int splitCount = std::min(amount, static_cast<int>(stackSize));
    ItemStack splitItem(item, splitCount);
    stackSize -= splitCount;
    return splitItem;
}


ND const lce::Item* ItemStack::getItem() const { return item; }


void ItemStack::addEnchantment(Enchantment* enchantment, int level) { enchantments.emplace_back(enchantment, level); }


void ItemStack::addEnchantmentData(EnchantmentData* enchantmentData) { enchantments.emplace_back(*enchantmentData); }
