#include "OceanMonumentPieces.hpp"

int OceanMonumentPieces::OceanMonumentPiece::GRIDROOM_SOURCE_INDEX = OceanMonumentPieces::OceanMonumentPiece::getRoomIndex(2, 0, 0);
int OceanMonumentPieces::OceanMonumentPiece::GRIDROOM_TOP_CONNECT_INDEX = OceanMonumentPieces::OceanMonumentPiece::getRoomIndex(2, 2, 0);
int OceanMonumentPieces::OceanMonumentPiece::GRIDROOM_LEFTWING_CONNECT_INDEX = OceanMonumentPieces::OceanMonumentPiece::getRoomIndex(0, 1, 0);
int OceanMonumentPieces::OceanMonumentPiece::GRIDROOM_RIGHTWING_CONNECT_INDEX = OceanMonumentPieces::OceanMonumentPiece::getRoomIndex(4, 1, 0);

OceanMonumentPieces::RoomDefinition OceanMonumentPieces::OceanMonumentPiece::NON_INITIALIZED_ROOM_DEF = OceanMonumentPieces::RoomDefinition(-1);