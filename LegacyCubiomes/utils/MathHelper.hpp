#pragma once

#include <cmath>

class MathHelper {
public:
    static float SIN_TABLE[65536];
    static void setup() {
        for (int i = 0; i < 65536; ++i)
        {
            SIN_TABLE[i] = (float)sinf((float)i / 10430.378F);
        }
    }

    /**
     * sin looked up in a table
     */
    static float sin(float value)
    {
        return SIN_TABLE[(int)(value * 10430.378F) & 65535];
    }

    /**
     * cos looked up in the sin table with the appropriate offset
     */
    static float cos(float value)
    {
        return SIN_TABLE[(int)(value * 10430.378F + 16384.0F) & 65535];
    }
};

float MathHelper::SIN_TABLE[65536];

