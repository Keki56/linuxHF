#include "gameengine.h"
#include <math.h>

#define g 9.81      //a gracitációs gyorsulás
#define MAX_V 100   //a lövedék maximális kezdősebessége

GameEngine::GameEngine() {}



void GameEngine::setLocalPlayer(double position, double angle, double power){
    localPlayer.position = position;
    localPlayer.angle = angle;
    localPlayer.power = power;
}

void GameEngine::setRemotePlayer(double position, double angle, double power) {
    remotePlayer.position = position;
    remotePlayer.angle = angle;
    remotePlayer.power = power;
}

int GameEngine::getLocalPlayerHp(){
    return localPlayer.hp;
}

int GameEngine::getRemotePlayerHp(){
    return remotePlayer.hp;
}

//Nincs még normálosan kész, de addig is visszaad valamit
bool GameEngine::fireLocalPlayer(){
    localPlayer.hp -= 10;
    return (localPlayer.hp > 0);
}

//Nincs még normálosan kész, de addig is visszaad valamit
bool GameEngine::fireRemotePlayer(){
    remotePlayer.hp -= 10;
    return (remotePlayer.hp > 0);
}
