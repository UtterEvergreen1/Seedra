#pragma once

#include "lce/processor.hpp"


enum GenerationStep : i8 {
    GS_NONE = 0,

    GS_Village_NONE = 0,
    GS_Village_Valid = 1,      // Generates the layout up until it makes the village valid
    GS_Village_Blacksmith = 2, // Generates the layout up to the blacksmith
    GS_Village_Layout = 3,     // Generates full layout
    GS_Village_Full = 4,       // Generates full layout and calculates hasMoreThanTwoComponents

    GS_Stronghold_NONE = 0,
    GS_Stronghold_Portal = 1, // Generates the layout up to the portal room, Y level will be inaccurate
    GS_Stronghold_Layout = 2, // Generates full layout, Y level will be inaccurate
    GS_Stronghold_Full = 3,   // Generates full layout and calculates Y level for all pieces



};











