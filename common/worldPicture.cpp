#include "worldPicture.hpp"
#include "lce/include/stb_image_write.hpp"


void WorldPicture::save(const std::string& directory) const {
    const std::string filename = getBiomeImageFileNameFromGenerator(m_g, directory);
    stbi_write_png(filename.c_str(), m_width, m_height,
        m_channels, m_data, m_width * m_channels);
}
