#include "enchantmentHelper.hpp"


//==============================================================================
//                              Enchant With Levels
//==============================================================================

void EnchantmentHelper::EnchantWithLevels::apply(uint64_t *rng, ItemStack *stack, int min, int max, bool isBook, bool isTreasure) {
    int rand = nextInt(rng, min, max);
    EnchantmentHelper::addRandomEnchantment(rng, stack, rand, isBook, isTreasure);
}

void EnchantmentHelper::EnchantWithLevels::apply(uint64_t *rng, ItemStack *stack, int level, bool isBook, bool isTreasure) {
    int rand = nextInt(rng, level, level);
    EnchantmentHelper::addRandomEnchantment(rng, stack, level, isBook, isTreasure);
}


//==============================================================================
//                              Enchant Randomly
//==============================================================================


void EnchantmentHelper::EnchantRandomly::apply(uint64_t *rng, ItemStack *stack) {
    Enchantment *enchantment;

    if (enchantments.empty()) {

        std::vector<Enchantment*> list;
        for (auto enchantment1 : Enchantment::REGISTRY) {
            if (stack->item == &Items::ENCHANTED_BOOK || enchantment1.second->canApply(stack->item)) {
                list.emplace_back(enchantment1.second);
            }
        }

        enchantment = (Enchantment*)list[nextInt(rng, (int)list.size())];

    } else {
        enchantment = (Enchantment*)&enchantments[nextInt(rng, (int)enchantments.size())];
    }

    int i = nextInt(rng, enchantment->minLevel, enchantment->maxLevel);

    stack->addEnchantment(enchantment, i);

}

//==============================================================================
//                              Enchantment Helpers
//==============================================================================

std::vector<EnchantmentData> EnchantmentHelper::buildEnchantmentList(uint64_t *rng, ItemStack *itemStackIn,
                                                         int level, bool isBook, bool allowTreasure) {
    std::vector<EnchantmentData> list;
    list.clear();

    const Item* item = itemStackIn->getItem();
    int i = item->getEnchantability();

    if (i <= 0)
        return list;

    level = level + 1 + nextInt(rng, i / 4 + 1) + nextInt(rng, i / 4 + 1);
    float f = (nextFloat(rng) + nextFloat(rng) - 1.0F) * 0.15F;
    level = clamp((int)std::round((float)level + (float)level * f), 1, std::numeric_limits<int>::max());

    std::vector<EnchantmentData> list1 = getEnchantmentDataList(level, itemStackIn, isBook, allowTreasure);
    /*std::cout << std::endl << "Enchant List" << std::endl;
    int count = 0;
    for (auto ench = list1.rbegin(); ench != list1.rend(); ++ench) {
        count += ench->obj->rarity->getWeight();
        std::cout << count << " " << ench->obj->name << std::endl;
    }
    std::cout << std::endl;*/

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
        if (!enchData.obj->canApplyTogether(it->obj)) { // I HAVE NO CLUE WHAT FUNCTION THIS ACTUALLY IS
            it = enchDataList.erase(it);
        } else {
            ++it;
        }
    }
}

void EnchantmentHelper::addRandomEnchantment(uint64_t *rng, ItemStack *itemStackIn, int level, bool isBook, bool allowTreasure) {
    std::vector<EnchantmentData> enchantmentList = buildEnchantmentList(rng, itemStackIn, level, isBook, allowTreasure);

    for (EnchantmentData enchantmentData : enchantmentList) {
        itemStackIn->addEnchantment(enchantmentData.obj, enchantmentData.level);
    }
}




std::vector<EnchantmentData> EnchantmentHelper::getEnchantmentDataList(int enchantmentLevelIn, ItemStack *ItemStackIn, bool isBook, bool allowTreasure) {
    std::vector<EnchantmentData> list;
    Enchantment *pointer = nullptr;
    bool allowed;

    isBook = ItemStackIn->getItem()->id == ENCHANTED_BOOK.id;

    for (auto & enchData : Enchantment::REGISTRY) {
        pointer = enchData.second;

        allowed = ((!pointer->isTreasure || allowTreasure)
                && (pointer->type->canEnchantItem(ItemStackIn->getItem()) || isBook));

        if (allowed) {
            for (int i = pointer->maxLevel; i > pointer->minLevel - 1; --i) { // minLevel can just be replaced with 1

                if (enchantmentLevelIn >= pointer->getMinEnchantability(i)
                    && enchantmentLevelIn <= pointer->getMaxEnchantability(i)) {
                    list.emplace_back(pointer, i);
                    break;
                }
            }
        }

    }

    return list;
}






