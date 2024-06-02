#pragma once

#include <cstdint>
#include <string>


namespace lce {

    enum class CONSOLE : int8_t {
        NONE = -1,
        XBOX360 = 0,
        PS3 = 1,
        WIIU = 2,
        VITA = 3,
        RPCS3 = 4,
        SWITCH = 5,
        PS4 = 6,
        XBOX1 = 7,
    };


    enum class DIMENSION : int8_t {
        NETHER = -1,
        HELL = NETHER,
        OVERWORLD = 0,
        END = 1,
        NONE = 2
    };


    enum class BIOMESCALE : int8_t {
        SMALL = 0,
        MEDIUM = 1,
        LARGE = 2
    };


    enum class WORLDSIZE : int8_t {
        CLASSIC = 0,
        SMALL = 1,
        MEDIUM = 2,
        LARGE = 3,
        INF = 4
    };


    static std::string consoleToStr(const CONSOLE console) {
        switch (console) {
            case CONSOLE::XBOX360:
                return "xbox360";
            case CONSOLE::PS3:
                return "ps3";
            case CONSOLE::RPCS3:
                return "rpcs3";
            case CONSOLE::WIIU:
                return "wiiu";
            case CONSOLE::VITA:
                return "vita";
            case CONSOLE::SWITCH:
                return "switch";
            case CONSOLE::PS4:
                return "ps4";
            case CONSOLE::XBOX1:
                return "xbox1";
            case CONSOLE::NONE:
            default:
                return "NONE";
        }
    }


    static CONSOLE strToConsole(const std::string& inputStr) {
        std::string str = inputStr;
        std::transform(str.begin(), str.end(), str.begin(),
                       [](unsigned char c){ return std::tolower(c); });

        if (str == "xbox360") {
            return CONSOLE::XBOX360;
        } else if (str == "ps3" || str == "playstation3") {
            return CONSOLE::PS3;
        } else if (str == "rpcs3") {
            return CONSOLE::RPCS3;
        } else if (str == "wiiu") {
            return CONSOLE::WIIU;
        } else if (str == "vita" || str == "psvita") {
            return CONSOLE::VITA;
        } else if (str == "switch") {
            return CONSOLE::SWITCH;
        } else if (str == "ps4" || str == "playstation4") {
            return CONSOLE::PS4;
        } else if (str == "xbox1" || str == "xboxone") {
            return CONSOLE::XBOX1;
        } else {
            return CONSOLE::NONE;
        }
    }


    static bool consoleIsBigEndian(const CONSOLE console) {
        switch (console) {
            case CONSOLE::NONE:
            case CONSOLE::XBOX360:
            case CONSOLE::PS3:
            case CONSOLE::RPCS3:
            case CONSOLE::WIIU:
            default:
                return true;
            case CONSOLE::VITA:
            case CONSOLE::PS4:
            case CONSOLE::SWITCH:
                return false;
        }
    }


    static int getChunkWorldBounds(const WORLDSIZE worldSize) {
        switch (worldSize) {
            case WORLDSIZE::CLASSIC:
            default:
                return 27;
            case WORLDSIZE::SMALL:
                return 32;
            case WORLDSIZE::MEDIUM:
                return 96;
            case WORLDSIZE::LARGE:
                return 160;
        }
    }


    static std::string worldSizeToString(const WORLDSIZE worldSize) {
        switch (worldSize) {
            case WORLDSIZE::CLASSIC:
            default:
                return "CLASSIC";
            case WORLDSIZE::SMALL:
                return "SMALL";
            case WORLDSIZE::MEDIUM:
                return "MEDIUM";
            case WORLDSIZE::LARGE:
                return "LARGE";
        }
    }


    static std::string biomeScaleToString(const BIOMESCALE biomeScale) {
        switch (biomeScale) {
            case BIOMESCALE::SMALL:
                return "SMALL";
            case BIOMESCALE::MEDIUM:
                return "MEDIUM";
            case BIOMESCALE::LARGE:
                return "LARGE";
            default:
                return "NONE";
        }
    }


}