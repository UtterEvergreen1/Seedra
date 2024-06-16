#include "lce/include/picture.hpp"
#include "lce/registry/blockRegistry.hpp"
#include "lce/registry/textureRegistry.hpp"

int main(int argc, char* argv[]) {
    lce::registry::TextureRegistry textures{};
    textures.setup();

    lce::registry::BlockRegistry blockReg{};
    blockReg.setup();

    Picture canvas(48, 48, 4);
    int sandID = blockReg.getBlockFromName("sand")->getID();
    auto const* sand = textures.getBlockFromID(sandID);

    int obsidianID = blockReg.getBlockFromName("obsidian")->getID();
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