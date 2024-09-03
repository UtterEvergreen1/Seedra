#include "enchantmentHelper.hpp"


EnchantedBookEnchantsLookupTable EnchantmentHelper::BOOK_LEVEL_TABLE;


void EnchantmentHelper::setConsoleAndVersion(lce::CONSOLE console, LCEVERSION version) {
    c_auto lceVersion = static_cast<i8>(version);

    // error handling
    if (Enchantment::tableOfOrders.find(console) == Enchantment::tableOfOrders.end())
        throw std::range_error("EnchantmentHelper: Console not implemented.");
    if (lceVersion >= Enchantment::tableOfOrders.at(console).size())
        throw std::range_error("EnchantmentHelper: Version not implemented.");
    if (Enchantment::currentConsole == console && Enchantment::currentVersion == version)
        return;

    Enchantment::currentConsole = console;
    Enchantment::currentVersion = version;
    Enchantment::REGISTRY.orderValues(Enchantment::tableOfOrders.at(console)[lceVersion]);

    BOOK_LEVEL_TABLE.deallocate();
    BOOK_LEVEL_TABLE.setup();
}


MU void EnchantmentHelper::deallocate() {
    BOOK_LEVEL_TABLE.deallocate();
    Enchantment::REGISTRY.clear();
    Enchantment::count = 0;
    Enchantment::isSetup = false;
}
