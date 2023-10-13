#include "enchantmentHelper.hpp"
#include "LegacyCubiomes/utils/MathHelper.hpp"
EnchantedBookEnchantsLookupTable EnchantmentHelper::BOOK_LEVEL_TABLE;

void EnchantmentHelper::setConsoleAndVersion(CONSOLE console, LCEVERSION version) {
    int8_t consoleNum = static_cast<int8_t>(console);
    int8_t lceVersion = static_cast<int8_t>(version);

    // error handling
    if(consoleNum >= Enchantment::tableOfOrders.size())
        throw std::range_error("Console not implemented.");
    if(lceVersion >= Enchantment::tableOfOrders.at(consoleNum).size())
        throw std::range_error("Version not implemented.");

    if(Enchantment::currentConsole == console && Enchantment::currentVersion == version)
        return;

    Enchantment::currentConsole = console;
    Enchantment::currentVersion = version;
    Enchantment::REGISTRY.orderValues(Enchantment::tableOfOrders[consoleNum][lceVersion]);

    BOOK_LEVEL_TABLE.deallocate();
    BOOK_LEVEL_TABLE.setup();
}

void EnchantmentHelper::deallocate() {
    BOOK_LEVEL_TABLE.deallocate();
    Enchantment::REGISTRY.clear();
    Enchantment::count = 0;
    Enchantment::isSetup = false;
}

