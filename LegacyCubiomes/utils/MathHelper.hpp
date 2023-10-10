#pragma once

#include <cmath>

class MathHelper {
public:
    float SIN_TABLE[65536];
    MathHelper() {
        for (int i = 0; i < 65536; ++i)
        {
            SIN_TABLE[i] = (float)sinf((float)i / 10430.378F);
        }
    }

    static MathHelper& Instance(){
        static MathHelper instance;
        return instance;
    }

    /**
     * sin looked up in a table
     */
    static float sin(float value)
    {
        return MathHelper::Instance().SIN_TABLE[(int)(value * 10430.378F) & 65535];
    }

    /**
     * cos looked up in the sin table with the appropriate offset
     */
    static float cos(float value)
    {
        return MathHelper::Instance().SIN_TABLE[(int)(value * 10430.378F + 16384.0F) & 65535];
    }
};

