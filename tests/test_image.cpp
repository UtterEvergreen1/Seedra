
#include "lce/include/picture.hpp"

int main(int argc, char* argv[]) {

    Picture picture{};
    picture.loadFromFile("redstone_ore.png");

    Picture sub(14, 14);
    picture.getSubImage(sub, 1, 1);

    picture.placeSubImage(&picture, 8, 8);

    picture.saveWithName("redstone_with_sub.png");



}