#include "enchantState.hpp"
#include "enchantController.hpp"

namespace enchants {

    /**
     * @brief Converts the enchantment state to a human-readable string.
     *
     * @return A std::string that includes the enchantment's name and its level in Roman numerals.
     *
     * @details
     * The function uses a static array of Roman numeral strings for levels 0 through 5.
     * It retrieves the corresponding Enchant object using EnchantController::getEnchantFromID().
     * If the retrieved object is null, it returns "NULLPTR 0". For enchantments with a maximum level of 1,
     * only the name is returned; otherwise, the name is concatenated with the level (in Roman numeral format).
     * If the stored level is out of the expected range, the level is defaulted to 0.
     */
    std::string EnchantState::toString() const {
        static const std::string LEVEL_ROMAN[6] = {"0", "I", "II", "III", "IV", "V"};
        const Enchant* obj = EnchantController::getEnchantFromID(this->getID());

        if EXPECT_FALSE (obj == nullptr) { return "NULLPTR 0"; }
        int theLevel = getLevel();
        if EXPECT_FALSE (getLevel() < 0 || getLevel() > 5) {
            theLevel = 0;
        }
        std::string result;
        if (obj->getMaxLevel() == 1) {
            result.reserve(obj->getName().size());
            result.append(obj->getName());
        } else {
            result.reserve(obj->getName().size() + 1 + LEVEL_ROMAN[theLevel].size());
            result.append(obj->getName());
            result.append(" ");
            result.append(LEVEL_ROMAN[theLevel]);
        }
        return result;
    }

} // namespace enchants
