#pragma once

#include "Container.hpp"
#include "common/rng.hpp"
#include "common/MathHelper.hpp"
#include "loot/enchants/enchant.hpp"
#include "loot/enchants/enchantController.hpp"

namespace loot {

    /**
     * @brief Applies metadata to an item state.
     *
     * @tparam ContainerSize The size of the container.
     * @tparam MinRoll The minimum roll value.
     * @tparam MaxRoll The maximum roll value.
     * @param container A reference to the container holding loot.
     * @param itemState The item state to modify.
     * @param rng The RNG instance used to generate a random number.
     *
     * @details The function calls @c rng.nextInt with the compile-time bounds @c MinRoll and @c MaxRoll,
     * and then uses the resulting value to set metadata in the item state by calling
     * @c itemState.setStateFromMeta.
     */
    template<int ContainerSize, u8 MinRoll, u8 MaxRoll>
    static void applyMetadata(MU Container<ContainerSize>& container, lce::ItemState& itemState, RNG& rng) {

        const u8 data = rng.nextInt<MinRoll, MaxRoll>();
        itemState.setStateFromMeta(data);
    }

    /**
     * @brief Randomly applies an enchant to a book.
     *
     * @tparam ContainerSize The size of the container.
     * @param container The container to receive the enchant.
     * @param itemStack The item state of the book.
     * @param random The RNG instance used to pick an enchant and level.
     *
     * @details This function selects a random enchant from the active enchantments
     * (via @c EnchantController::getActiveEnchants), obtains a random level between 1 and the enchant's
     * maximum level, and then adds the enchant to the container. The enchant is represented by its ID.
     *
     * @note The enchant is applied only to enchanted books.
     */
    template<int ContainerSize>
    static void applyEnchantRandomlyBook(Container<ContainerSize>& container, MU lce::ItemState& itemStack, RNG& random) {
        using namespace enchants;
        const Enchant* enchPtr = EnchantController::getActiveEnchants()[
                random.nextInt((int)EnchantController::getActiveSize())
        ];
        c_int level = random.nextInt(1, enchPtr->getMaxLevel());
        container.addEnchant({enchPtr->getID(), level, 0});
    }

    /**
     * @brief Randomly applies an enchant to an item.
     *
     * @tparam ContainerSize The size of the container.
     * @param container The container that will receive the enchantment.
     * @param itemIn The item state to be enchanted.
     * @param random The RNG instance used to choose an enchant and level.
     *
     * @details The function iterates through all active enchantments and collects those that can be applied to
     * the item (using @c canApply). Then, it randomly selects one of those enchantments and a random level,
     * and finally adds the enchantment to the container using the enchant's ID.
     */
    template<int ContainerSize>
    static void applyEnchantRandomlyItem(Container<ContainerSize>& container, lce::ItemState& itemIn, RNG& random) {
        using namespace enchants;
        // leave this uninitialized
        std::array<const Enchant*, MAX_ENCHANTMENT_COUNT> tempEnchants;
        int index = 0;
        for (const Enchant* EnchantPointer: EnchantController::getActiveEnchants()) {
            if (EnchantPointer->canApply(itemIn)) {
                tempEnchants[index] = EnchantPointer;
                index++;
            }
        }

        const Enchant* enchPtr = tempEnchants[random.nextInt(index)];
        c_int level = random.nextInt(1, enchPtr->getMaxLevel());
        container.addEnchant({enchPtr->getID(), level, 0});
    }

    /**
     * @brief Applies an enchant with levels to an item.
     *
     * @tparam ContainerSize The size of the container.
     * @tparam MinRoll The minimum roll for generating a base level.
     * @tparam MaxRoll The maximum roll for generating a base level.
     * @param container The container to which the enchant is added.
     * @param itemIn The item state to enchant.
     * @param rng The RNG instance used to generate the enchant level.
     *
     * @details
     * This function computes an "XP level" for the enchantment by generating a base value using
     * @c rng.nextInt, then adding additional random values based on the item's cost. The resulting XP
     * level is clamped and used to determine which enchantments are valid by comparing it against the
     * minimum and maximum cost functions of each enchant. Valid enchantments are collected in an array,
     * and one is chosen based on their weighted rarity.
     *
     * @note This function distinguishes between enchanted books and other items, processing them slightly differently.
     */
    template<int ContainerSize, u8 MinRoll, u8 MaxRoll>
    static void applyEnchantWithLevelsItem(Container<ContainerSize>& container, lce::ItemState& itemIn, RNG& rng) {
        using namespace enchants;
        c_int levelStart = rng.nextInt<MinRoll, MaxRoll>();
        int xpLevel = levelStart;

        c_int cost = (itemIn.getCost() >> 2) + 1;

        xpLevel += 1 + rng.nextInt(cost) + rng.nextInt(cost);
        const auto levelF = static_cast<float>(xpLevel);
        c_float f = (rng.nextFloat() + rng.nextFloat() - 1.0F) * 0.15F;
        xpLevel = MathHelper::clamp(static_cast<int>(std::round(levelF + levelF * f)), 1, 0x7fffffff);

        std::array<EnchantState, MAX_ENCHANTMENT_COUNT> activeList;
        std::array<bool, MAX_ENCHANTMENT_COUNT> validFlags = {};
        int activeListSize = 0;

        if (itemIn.getID() == lce::items::ids::ENCHANTED_BOOK_ID) {
            for (const Enchant* enchPtr : EnchantController::getActiveEnchants()) {
                for (u8 i = enchPtr->getMaxLevel(); i > 0; --i) {
                    if (xpLevel >= enchPtr->getMinCost(i) && xpLevel <= enchPtr->getMaxCost(i)) {
                        activeList[activeListSize] = EnchantState(enchPtr->getID(), i, (int)container.slotCount());
                        validFlags[activeListSize] = true;
                        activeListSize++;
                        break;
                    }
                }
            }
        } else {

            for (const Enchant* enchPtr : EnchantController::getActiveEnchants()) {
                if (!canEnchantItem(itemIn, enchPtr->getType())) {
                    continue;
                }
                for (u8 level = enchPtr->getMaxLevel(); level > 0; --level) {
                    if (xpLevel >= enchPtr->getMinCost(level) && xpLevel <= enchPtr->getMaxCost(level)) {
                        activeList[activeListSize] = EnchantState(enchPtr->getID(), level, (int)container.slotCount());
                        validFlags[activeListSize] = true;
                        activeListSize++;
                        break;
                    }
                }
            }
        }

        EnchantState back;
        int totalWeight = 0;

        for (int i = 0; i < activeListSize; ++i) {
            if (validFlags[i]) {
                totalWeight += EnchantController::getWeightFromID(activeList[i].getID());
            }
        }

        if (activeListSize == 0) return;

        int weight1 = rng.nextInt(totalWeight);
        for (int i = 0; i < activeListSize; ++i) {
            if (validFlags[i]) {
                weight1 -= EnchantController::getWeightFromID(activeList[i].getID());
                if (weight1 < 0) {
                    back = activeList[i];
                    container.addEnchant(back);
                    break;
                }
            }
        }

        const Enchant* backEnch = EnchantController::getEnchantFromID(back.getID());
        while (rng.nextInt(50) <= xpLevel) {
            for (int i = 0; i < activeListSize; ++i) {
                const Enchant* activeEnch = EnchantController::getEnchantFromID(activeList[i].getID());
                if (validFlags[i] && !backEnch->canApplyTogether(activeEnch)) {
                    validFlags[i] = false;
                }
            }

            totalWeight = 0;
            for (int i = 0; i < activeListSize; ++i) {
                if (validFlags[i]) {
                    totalWeight += EnchantController::getWeightFromID(activeList[i].getID());
                }
            }

            if (totalWeight == 0) break;

            int weight2 = rng.nextInt(totalWeight);
            for (int i = 0; i < activeListSize; ++i) {
                if (validFlags[i]) {
                    weight2 -= EnchantController::getWeightFromID(activeList[i].getID());
                    if (weight2 < 0) {
                        back = activeList[i];
                        container.addEnchant(back);
                        break;
                    }
                }
            }
            xpLevel /= 2;
        }
    }

    /**
     * @brief Builds a lookup table for book enchantments.
     *
     * @param itemIn The enchanted book's item state.
     * @param rng The RNG instance for randomization.
     * @param level A base level used for generating enchantments.
     * @return A pointer to an @c EnchantLevelArray that contains the applicable enchantments.
     *
     * @details
     * This function calculates an effective level by adding random increments to the provided base level,
     * clamps the value, and then retrieves an enchantment level array from the enchant controller.
     * It then repeatedly adds random enchantments to the array while reducing the level, simulating the
     * cascading effect of enchantments on a book.
     *
     * @warning The cost value must be within valid bounds (typically 0 to 47) or the behavior is undefined.
     */
    static enchants::EnchantLevelArray* builtEnchantListForBook(const lce::ItemState& itemIn, RNG& rng, int level) {
        using namespace enchants;
        c_int cost = (itemIn.getCost() >> 2) + 1;

        level += 1 + rng.nextInt(cost) + rng.nextInt(cost);
        const auto levelF = static_cast<float>(level);
        c_float f = (rng.nextFloat() + rng.nextFloat() - 1.0F) * 0.15F;
        level = MathHelper::clamp(static_cast<int>(std::round(levelF + levelF * f)), 1, 0x7fffffff);

        EnchantLevelArray* enchants = EnchantController::getLevelArray(level);
        enchants->addRandomItem(rng);

        while (rng.nextInt(50) <= level) {
            const EnchantState back = enchants->getLastEnchantAdded();
            const Enchant* backEnch = EnchantController::getEnchantFromID(back.getID());

            for (int enchIndex = 0; enchIndex < enchants->totalEnchants; enchIndex++) {
                const Enchant* activeEnch = EnchantController::getEnchantFromID(enchants->data[enchIndex].getID());
                if (!backEnch->canApplyTogether(activeEnch)) {
                    // std::cout << enchants->data[enchIndex].obj->name << " removed" << std::endl;
                    for (int i = 0; i < enchants->deletions.getIndex(); i++)
                        if (enchIndex == enchants->deletions.getValueAt(i)) goto END;
                    enchants->deletions.addItem(enchIndex);
                }
            END:;
            }

            enchants->addRandomItem(rng);
            level /= 2;
        }
        return enchants;
    }

    /**
     * @brief Applies enchantments with levels to a book.
     *
     * @tparam ContainerSize The size of the container.
     * @tparam MinRoll The minimum roll value for generating the base level.
     * @tparam MaxRoll The maximum roll value for generating the base level.
     * @param container The container to which the enchant is added.
     * @param itemIn The item state of the enchanted book.
     * @param random The RNG instance used for determining enchant levels.
     *
     * @details
     * This function generates a base level using @c rng.nextInt, builds an enchantment lookup array by
     * calling @c builtEnchantListForBook, and then iterates over the lookup array to add the resulting
     * enchantments (by their ID) to the container.
     */
    template<int ContainerSize, u8 MinRoll, u8 MaxRoll>
    MU static void applyEnchantWithLevelsBook(Container<ContainerSize>& container, lce::ItemState& itemIn, RNG& random) {
        using namespace enchants;
        c_int level = random.nextInt<MinRoll, MaxRoll>();
        const EnchantLevelArray* EnchantVector = builtEnchantListForBook(itemIn, random, level);

        for (int i = 0; i < EnchantVector->enchants.getIndex(); i++) {
            auto& eData = EnchantVector->data[EnchantVector->enchants.getValueAt(i)];
            container.addEnchant({eData.getID(), eData.getLevel(), (int)container.slotCount()});
        }
    }

    /**
     * @brief Typedef for a function that applies an enchant to an item.
     *
     * @details The function pointer type @c LootApplyFunc represents a function that takes a container,
     * an item state, and an RNG reference, and applies an enchantment to the item.
     */
    typedef void (*LootApplyFunc)(Container<27>&, lce::ItemState&, RNG&);

    /**
     * @brief Constant function pointers for various enchantment applications.
     *
     * These constants allow the loot system to refer to specific enchant application functions.
     */
    static constexpr LootApplyFunc metadata_0_3 = &applyMetadata<27, 0, 3>;
    static constexpr LootApplyFunc metadata_0_1 = &applyMetadata<27, 0, 1>;
    static constexpr LootApplyFunc enchant_random_item = &applyEnchantRandomlyItem<27>;
    static constexpr LootApplyFunc enchant_random_book = &applyEnchantRandomlyBook<27>;
    static constexpr LootApplyFunc enchant_levels_item_20_39 = &applyEnchantWithLevelsItem<27, 20, 39>;
    static constexpr LootApplyFunc enchant_levels_item_30 = &applyEnchantWithLevelsItem<27, 30, 30>;
    static constexpr LootApplyFunc enchant_levels_book_30 = &applyEnchantWithLevelsBook<27, 30, 30>;

} // namespace loot