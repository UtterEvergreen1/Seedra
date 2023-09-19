To use this code,

do

```c++
#include "LegacyCubiomes/enchants/enchantmentHelper.hpp"

int main() {
    Enchantment::registerEnchantments();
    // ...
}
```
The code will not work if ``registerEnchantments`` is not called.

these are the functions to use for enchanting:

```c++
// EnchantmentHelper::EnchantWithLevels::
static void apply(uint64_t *rng, ItemStack *stack, int min, int max, bool isBook, bool allowTreasure);
static void apply(uint64_t *rng, ItemStack *stack, int level, bool isBook, bool allowTreasure);
// EnchantmentHelper::EnchantRandomly::
static void apply(uint64_t *rng, ItemStack *stack);
```

btw im not sure if ``EnchantRandomly`` passes a reference to rng or not.

There are compiler flags for ``INCLUDE_JAVA``, and I know it works for Java completely.
<br>
This flag adds the ``SweepingEdge`` enchantment to the registry.

rn it doesn't correctly shuffle Java loot, idk why