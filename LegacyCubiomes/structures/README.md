
# How to Use
...

# Generation
...









# Placement
```c++
// all examples given below are called "Placement::..."
using namespace Placement;

// static
Feature();
Village<false>(); // <= 1.13
Village<true>();  // >= 1.14
OceanRuin();

// dynamic
Mansion();
Monument();
Treasure();
Shipwreck();
Outpost();
``` 
### Static Structures
```c++
// features
StructureType Feature::getFeatureType(Generator* g, (int, int | Pos2D));
std::vector<FeatureStructurePair> Feature::getAllFeaturePositions(Generator* g);

// features, ocean ruin, villages
static void setWorldSize(WORLDSIZE worldSize);
```












# Rolls:
...





