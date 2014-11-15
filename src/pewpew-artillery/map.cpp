#include "map.h"

#define MAP_SIZE 1000       //a pálya szélessége
#define USER_SPACE 0.3      //a pályának mekkora részén mozoghatnak a játékosok
#define WALL_TOP 400
#define WALL_WIDTH 50

Map::Map() {}

double Map::getDistance(double localPlayerPos, double remotePlayerPos) {
    return MAP_SIZE * USER_SPACE * (remotePlayerPos - localPlayerPos) + MAP_SIZE - 2*USER_SPACE;
}
