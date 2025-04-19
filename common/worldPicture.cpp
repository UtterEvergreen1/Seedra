#include "worldPicture.hpp"
#include "lce/include/stb_image_write.h"


void WorldPicture::save(const std::string& directory) const {
    const std::string filename = getBiomeImageFileNameFromGenerator(g, directory);
    stbi_write_png(filename.c_str(), static_cast<int>(myWidth), static_cast<int>(myHeight),
        static_cast<int>(myRGBSize), myData,
                   static_cast<int>(myWidth * myRGBSize));
}