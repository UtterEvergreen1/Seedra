#include "lce/blocks/block_ids.hpp"
#include "lce/texture/TextureMap.hpp"

int main(int argc, char* argv[]) {
    TextureMap map;
    map.setup();

    Picture const* pic = map.BLOCK_TEXTURES
         .getObjectById(lce::blocks::ids::GRASS_ID);

    pic->saveWithName("grass_texture.png", "");
    return 0;
}