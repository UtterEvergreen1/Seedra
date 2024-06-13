#include "lce/blocks/block.hpp"
#include "lce/blocks/block_ids.hpp"
#include "lce/texture/textureMap.hpp"

int main(int argc, char* argv[]) {
    TextureMap textures;
    textures.setup();

    Picture canvas(48, 48, 4);
    int sandID = lce::blocks::REGISTRY.getObjFromName("sand")->getID();
    auto const* sand = textures.getBlockFromID(sandID);

    int obsidianID = lce::blocks::REGISTRY.getObjFromName("obsidian")->getID();
    auto const* obsidian = textures.getBlockFromID(obsidianID);

    canvas.placeSubImage(sand, 0, 0);
    canvas.placeSubImage(obsidian, 16, 0);
    canvas.placeSubImage(sand, 32, 0);
    canvas.placeSubImage(obsidian, 0, 16);
    canvas.placeSubImage(sand, 16, 16);
    canvas.placeSubImage(obsidian, 32, 16);
    canvas.placeSubImage(sand, 0, 32);
    canvas.placeSubImage(obsidian, 16, 32);
    canvas.placeSubImage(sand, 32, 32);

    canvas.saveWithName("sillyDrawing.png", "");
    return 0;
}