#include "enchantmentHelper.hpp"


EnchantedBookEnchantsLookupTable EnchantmentHelper::BOOK_LEVEL_TABLE;


void EnchantmentHelper::setConsoleAndVersion(lce::CONSOLE console, LCEVERSION version) {
    const auto consoleNum = static_cast<int8_t>(console);
    const auto lceVersion = static_cast<int8_t>(version);

    // error handling
    if (consoleNum >= Enchantment::tableOfOrders.size())
        throw std::range_error("Console not implemented.");
    if (lceVersion >= Enchantment::tableOfOrders.at(consoleNum).size())
        throw std::range_error("Version not implemented.");

    if (Enchantment::currentConsole == console && Enchantment::currentVersion == version) return;

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
