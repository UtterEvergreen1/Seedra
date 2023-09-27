#include "ItemStack.hpp"


ItemStack ItemStack::splitStack(int amount) {
    int splitCount = std::min(amount, this->stackSize);
    ItemStack splitItem(this->item, splitCount);
    this->stackSize -= splitCount;
    return splitItem;
}


ND const Items::Item* ItemStack::getItem() const {
    return this->item;
}


void ItemStack::addEnchantment(Enchantment* enchantment, int level) {
    enchantments.emplace_back(enchantment, level);
}


void ItemStack::addEnchantmentData(EnchantmentData* enchantmentData) {
    enchantments.emplace_back(*enchantmentData);
}
