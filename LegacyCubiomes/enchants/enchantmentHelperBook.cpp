#include "enchantmentHelperBook.hpp"

EnchantedBookEnchantsLookupTable EnchantmentHelperBook::BOOK_LEVEL_TABLE;

//==============================================================================
//                              Enchant With Levels
//==============================================================================


void EnchantmentHelperBook::EnchantWithLevels::apply(uint64_t *rng, ItemStack *stack, int min, int max) {
    int rand = nextInt(rng, min, max);
    EnchantmentHelperBook::addRandomEnchantment(rng, stack, rand);
}


void EnchantmentHelperBook::EnchantWithLevels::apply(uint64_t *rng, ItemStack *stack, int level) {
    int rand = nextInt(rng, level, level);
    EnchantmentHelperBook::addRandomEnchantment(rng, stack, level);
}


//==============================================================================
//                              Enchantment Helpers
//==============================================================================


ELDataArray* EnchantmentHelperBook::buildEnchantmentList(uint64_t *rng, ItemStack *itemStackIn, int level) {

    const Items::Item* item = itemStackIn->getItem();
    int cost = item->getCost();

    level = level + 1 + nextInt(rng, cost / 4 + 1) + nextInt(rng, cost / 4 + 1);
    float f = (nextFloat(rng) + nextFloat(rng) - 1.0F) * 0.15F;
    level = clamp((int)std::round((float)level + (float)level * f), 1, std::numeric_limits<int>::max()); // 0x7fffffff

    std::cout << "Level: " << level << std::endl;

    ELDataArray* enchants = getEnchantmentDataList(level, itemStackIn);
    enchants->addRandomItem(rng);

    while (nextInt(rng, 50) <= level) {
        EnchantmentData* back = enchants->getLastEnchantmentAdded();

        int count = 0;
        bool canAdd;

        for (int enchIndex = 0; enchIndex < enchants->totalEnchants; enchIndex++) {

            canAdd = true;
            if (!back->obj->canApplyTogether(enchants->data[enchIndex].obj)) {

                for (int i = 0; i < enchants->deletions.getIndex(); i++) {
                    if (count == enchants->deletions.getValueAt(i)) {
                        canAdd = false;
                        break;
                    }
                }

                if (canAdd) {
                    enchants->deletions.addItem(count);
                }
            }
            count++;
        }

        enchants->addRandomItem(rng);
        level /= 2;
    }

    return enchants;
}

void EnchantmentHelperBook::addRandomEnchantment(uint64_t *const rng, ItemStack *const itemStackIn, int level) {
    ELDataArray* enchantmentVector = buildEnchantmentList(rng, itemStackIn, level);
    enchantmentVector->addEnchantments(itemStackIn);
}


ELDataArray* EnchantmentHelperBook::getEnchantmentDataList(int enchantCost, ItemStack *ItemStackIn) {
    return BOOK_LEVEL_TABLE.get(enchantCost);
}
