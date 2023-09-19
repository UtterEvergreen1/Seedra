#include "enchantmentHelper.hpp"


//==============================================================================
//                              Enchant With Levels
//==============================================================================

template<bool isBook, bool allowTreasure>
void EnchantmentHelper::EnchantWithLevels::apply(uint64_t *rng, ItemStack *stack, int min, int max) {
    int rand = nextInt(rng, min, max);
    EnchantmentHelper::addRandomEnchantment<isBook, allowTreasure>(rng, stack, rand);
}

template<bool isBook, bool allowTreasure>
void EnchantmentHelper::EnchantWithLevels::apply(uint64_t *rng, ItemStack *stack, int level) {
    int rand = nextInt(rng, level, level);
    EnchantmentHelper::addRandomEnchantment<isBook, allowTreasure>(rng, stack, level);
}


//==============================================================================
//                              Enchant Randomly
//==============================================================================


template<bool isBook>
void EnchantmentHelper::EnchantRandomly::apply(uint64_t *rng, ItemStack *stack) {
    Enchantment *enchantmentPointer;

    if (enchantments.empty()) {
        std::vector<Enchantment*> list;
        for (auto it : Enchantment::REGISTRY) {
            if (stack->item == &Items::ENCHANTED_BOOK || it.second->canApply(stack->item)) {
                list.emplace_back(it.second);
            }
        }

        enchantmentPointer = list[nextInt(rng, (int)list.size())];

    } else {
        enchantmentPointer = &enchantments[nextInt(rng, (int)enchantments.size())];
    }

    int i = nextInt(rng, enchantmentPointer->minLevel, enchantmentPointer->maxLevel);

    stack->addEnchantment(enchantmentPointer, i);

}

//==============================================================================
//                              Enchantment Helpers
//==============================================================================

template<bool isBook, bool allowTreasure>
std::vector<EnchantmentData> EnchantmentHelper::buildEnchantmentList(uint64_t *rng, ItemStack *itemStackIn, int level) {
    std::vector<EnchantmentData> list;
    list.reserve(MAX_ENCHANT_LIST_SIZE);

    const Items::Item* item = itemStackIn->getItem();
    int i = item->getCost();

    if (i == 0)
        return list;

    level = level + 1 + nextInt(rng, i / 4 + 1) + nextInt(rng, i / 4 + 1);
    float f = (nextFloat(rng) + nextFloat(rng) - 1.0F) * 0.15F;
    level = clamp((int)std::round((float)level + (float)level * f), 1, std::numeric_limits<int>::max()); // 0x7fffffff

    std::cout << "Level: " << level << std::endl;

    std::vector<EnchantmentData> list1 = getEnchantmentDataList<isBook, allowTreasure>(level, itemStackIn);

    if (!list1.empty()) {
        EnchantmentData data = WeightedRandom::getRandomItem(rng, list1);
        list.push_back(data);
        while (nextInt(rng, 50) <= level) {
            removeIncompatible(list1, list.back());
            if (list1.empty())
                break;
            data = WeightedRandom::getRandomItem(rng, list1);
            list.push_back(data);
            level /= 2;
        }
    }
    return list;
}

void EnchantmentHelper::removeIncompatible(std::vector<EnchantmentData>& enchDataList, EnchantmentData enchData) {
    for (auto it = enchDataList.begin(); it != enchDataList.end(); ) {
        if (!enchData.obj->canApplyTogether(it->obj)) {
            it = enchDataList.erase(it);
        } else {
            ++it;
        }
    }
}

template<bool isBook, bool allowTreasure>
void EnchantmentHelper::addRandomEnchantment(uint64_t *const rng, ItemStack *const itemStackIn, int level) {
    std::vector<EnchantmentData> enchantmentList = buildEnchantmentList<isBook, allowTreasure>(rng, itemStackIn, level);

    for (const auto &enchantmentData : enchantmentList) {
        itemStackIn->addEnchantment(enchantmentData.obj, enchantmentData.level);
    }
}



template<bool isBook, bool allowTreasure>
std::vector<EnchantmentData> EnchantmentHelper::getEnchantmentDataList(int enchantmentLevelIn, ItemStack *ItemStackIn) {
    std::vector<EnchantmentData> list;
    Enchantment *pointer = nullptr;

    for (auto & it : Enchantment::REGISTRY) {
        pointer = it.second;

        if (pointer->isTreasure && !allowTreasure) continue;
        if (!pointer->type->canEnchantItem(ItemStackIn->getItem()) && !isBook) continue;

        for (int i = pointer->maxLevel; i > 0; --i) { // maxLevel to minLevel - 1 (always 0)

            if (enchantmentLevelIn >= pointer->getMinCost(i)
                && enchantmentLevelIn <= pointer->getMaxCost(i)) {
                list.emplace_back(pointer, i);
                break;
            }
        }
    }

    return list;
}



template void EnchantmentHelper::EnchantWithLevels::apply<true, true>(uint64_t*, ItemStack*, int);
template void EnchantmentHelper::EnchantWithLevels::apply<false, true>(uint64_t*, ItemStack*, int);
template void EnchantmentHelper::EnchantWithLevels::apply<true, false>(uint64_t*, ItemStack*, int);
template void EnchantmentHelper::EnchantWithLevels::apply<false, false>(uint64_t*, ItemStack*, int);


