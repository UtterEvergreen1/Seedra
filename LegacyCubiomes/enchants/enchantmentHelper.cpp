#include "enchantmentHelper.hpp"


//==============================================================================
//                              Enchant With Levels Items
//==============================================================================


template<bool allowTreasure>
void EnchantWithLevelsItem::apply(uint64_t *rng, ItemStack *stack, int min, int max) {
    int rand = nextInt(rng, min, max);
    addRandomEnchantment<allowTreasure>(rng, stack, rand);
}
template void EnchantWithLevelsItem::apply<true>(uint64_t*, ItemStack*, int, int);
template void EnchantWithLevelsItem::apply<false>(uint64_t*, ItemStack*, int, int);




template<bool allowTreasure>
void EnchantWithLevelsItem::apply(uint64_t *rng, ItemStack *stack, int level) {
    addRandomEnchantment<allowTreasure>(rng, stack, level);
}
template void EnchantWithLevelsItem::apply<true>(uint64_t *, ItemStack *, int);
template void EnchantWithLevelsItem::apply<false>(uint64_t *, ItemStack *, int);


template<bool allowTreasure>
EnchDataVec_t EnchantWithLevelsItem::buildEnchantmentList(uint64_t *rng, ItemStack *itemStackIn, int level) {
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

    std::vector<EnchantmentData> list1 = getEnchantmentDataList<allowTreasure>(level, itemStackIn);

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
template EnchDataVec_t EnchantWithLevelsItem::buildEnchantmentList<false>(uint64_t*, ItemStack*, int);
template EnchDataVec_t EnchantWithLevelsItem::buildEnchantmentList<true>(uint64_t*, ItemStack*, int);



void EnchantWithLevelsItem::removeIncompatible(EnchDataVec_t& enchDataList, EnchantmentData enchData) {
    std::cout << "REMOVE_INCOMPATIBLE" << std::endl;

    for (auto it = enchDataList.begin(); it != enchDataList.end(); ) {
        if (!enchData.obj->canApplyTogether(it->obj)) {
            std::cout << it->obj->name << " removed" << std::endl;
            it = enchDataList.erase(it);
        } else {
            ++it;
        }
    }
}



template<bool allowTreasure>
void EnchantWithLevelsItem::addRandomEnchantment(uint64_t *const rng, ItemStack *const itemStackIn, int level) {
    EnchDataVec_t enchantmentList = buildEnchantmentList<allowTreasure>(rng, itemStackIn, level);

    for (const auto &enchantmentData : enchantmentList) {
        itemStackIn->addEnchantment(enchantmentData.obj, enchantmentData.level);
    }
}
template void EnchantWithLevelsItem::addRandomEnchantment<true>(uint64_t*, ItemStack*, int);
template void EnchantWithLevelsItem::addRandomEnchantment<false>(uint64_t*, ItemStack*, int);



template<bool allowTreasure>
EnchDataVec_t EnchantWithLevelsItem::getEnchantmentDataList(int enchantmentLevelIn, ItemStack *ItemStackIn) {
    EnchDataVec_t list;
    Enchantment *pointer = nullptr;
    bool added;

    for (auto & it : Enchantment::REGISTRY) {
        added = false;
        pointer = it.second;

        if (pointer->isTreasure && !allowTreasure) continue;
        if (!pointer->type->canEnchantItem(ItemStackIn->getItem())) continue;

        for (int i = pointer->maxLevel; i > 0; --i) { // maxLevel to minLevel - 1 (always 0)

            if (enchantmentLevelIn >= pointer->getMinCost(i)
                && enchantmentLevelIn <= pointer->getMaxCost(i)) {
                list.emplace_back(pointer, i);
                added = true;
                break;
            }
        }

        if (!added) {
            std::cout << "failed to add " << pointer->name << std::endl;
        }
    }

    return list;
}
template EnchDataVec_t EnchantWithLevelsItem::getEnchantmentDataList<false>(int , ItemStack*);
template EnchDataVec_t EnchantWithLevelsItem::getEnchantmentDataList<true>(int , ItemStack*);


//==============================================================================
//                      Enchant With Levels EnchantedBook
//==============================================================================



template<bool allowTreasure>
void EnchantWithLevelsBook::apply(uint64_t *rng, ItemStack *stack, int level) {
    ELDataArray* enchantmentVector = buildEnchantmentList(rng, stack, level);
    enchantmentVector->addEnchantments(stack);
}
template void EnchantWithLevelsBook::apply<false>(uint64_t*, ItemStack*, int);
template void EnchantWithLevelsBook::apply<true>(uint64_t*, ItemStack*, int);



ELDataArray* EnchantWithLevelsBook::buildEnchantmentList(uint64_t *rng, ItemStack *itemStackIn, int level) {

    const Items::Item* item = itemStackIn->getItem();
    int cost = (item->getCost() >> 2) + 1;

    level = level + 1 + nextInt(rng, cost) + nextInt(rng, cost);
    float f = (nextFloat(rng) + nextFloat(rng) - 1.0F) * 0.15F;
    level = clamp((int)std::round((float)level + (float)level * f), 1, 0x7fffffff);

    ELDataArray* enchants = BOOK_LEVEL_TABLE.get(level);
    enchants->addRandomItem(rng);

    while (nextInt(rng, 50) <= level) {
        EnchantmentData* back = enchants->getLastEnchantmentAdded();

        for (int enchIndex = 0; enchIndex < enchants->totalEnchants; enchIndex++) {
            if (!back->obj->canApplyTogether(enchants->data[enchIndex].obj)) {
                // std::cout << enchants->data[enchIndex].obj->name << " removed" << std::endl;
                for (int i = 0; i < enchants->deletions.getIndex(); i++)
                    if (enchIndex == enchants->deletions.getValueAt(i))
                        goto END;
                enchants->deletions.addItem(enchIndex);
            }
            END:;
        }

        enchants->addRandomItem(rng);
        level /= 2;
    }
    return enchants;
}


//==============================================================================
//                              Enchant Randomly Items
//==============================================================================



void EnchantRandomlyItem::apply(uint64_t *rng, ItemStack *stack) {
    Enchantment *enchantmentPointer;

    std::vector<Enchantment*> list;
    for (auto it : Enchantment::REGISTRY)
        if (stack->item->getID() == Items::ENCHANTED_BOOK_ID || it.second->canApply(stack->item))
            list.emplace_back(it.second);

    int rand = nextInt(rng, (int)list.size());
    enchantmentPointer = list[rand];

    int level = nextInt(rng, enchantmentPointer->minLevel, enchantmentPointer->maxLevel);

    stack->addEnchantment(enchantmentPointer, level);
}


//==============================================================================
//                              Enchant Randomly Books
//==============================================================================



void EnchantRandomlyBook::apply(uint64_t *rng, ItemStack *stack) {
    Enchantment *enchantmentPointer;
    int rand = nextInt(rng, (int)Enchantment::REGISTRY.size());
    enchantmentPointer = Enchantment::REGISTRY[rand];
    int level = nextInt(rng, enchantmentPointer->minLevel, enchantmentPointer->maxLevel);
    stack->addEnchantment(enchantmentPointer, level);
}

