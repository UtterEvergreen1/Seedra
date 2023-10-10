[[OUT_DATED]]

To use this code,

do

```c++
#include "LegacyCubiomes/enchants/include.hpp"

int main() {
    EnchantmentHelper::setup(CONSOLE::WIIU, LCEVERSION::AQUATIC);
    // ...
}
```
Any code relating to loot gen that uses enchantments will 
not work if ``EnchantmentHelper::setup`` is not called.

these are the functions to use for enchanting:

```c++
// EnchantmentHelper::EnchantWithLevels::
static void apply(uint64_t *rng, ItemStack *stack, int min, int max, bool isBook);
static void apply(uint64_t *rng, ItemStack *stack, int level, bool isBook);
// EnchantmentHelper::EnchantRandomly::
static void apply(uint64_t *rng, ItemStack *stack);
```

rn it doesn't correctly shuffle Java loot, idk why