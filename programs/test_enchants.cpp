
#include "lce/include/picture.hpp"
#include "LegacyCubiomes/enchants/enchantmentHelper.hpp"
#include "LegacyCubiomes/loot/tables/include.hpp"


static inline void setSeed(uint64_t *seed, uint64_t value)
{
    *seed = (value ^ 0x5deece66d) & 0xFFFFFFFFFFFF;
}

int main(int argc, char* argv[]) {

    EnchantmentHelper::setup(lce::CONSOLE::WIIU, LCEVERSION::ELYTRA);
    loot::StrongholdLibrary::setup();

    uint64_t lootTableSeed = 216765366;
    Container loot;
    loot = loot::StrongholdLibrary::getLootFromLootTableSeed<false>(lootTableSeed);
    std::cout << loot << std::endl;
}