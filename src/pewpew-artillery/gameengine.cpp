#include "gameengine.h"
#include <math.h>

//A pálya 0.0-tól 1.0-ig tart

#define g 9.81                  //a gracitációs gyorsulás
#define MAX_V 100               //a lövedék maximális kezdősebessége

#define WALL_LEFT 0.45          //a fal bal széle
#define WALL_RIGHT 0.55         //a fal jobb széle
#define WALL_TOP 0.3            //a fal magassága
#define DEFAULT_POSITION 0.2    //a játékosok alapértelmezett pozíciója     |-x-P1-----WALL-----P2-x-|
#ifndef M_PI_4
#define M_PI_4		0.78539816339744830962
#endif

GameEngine::GameEngine(/*bool localStarts): isLocalTurn(localStarts*/) {
    wall = Wall();
    wall.left = WALL_LEFT;
    wall.right = WALL_RIGHT;
    wall.top = WALL_TOP;

    setLocalPlayer(DEFAULT_POSITION, M_PI_4, 0);
    setLocalPlayer(1.0 - DEFAULT_POSITION, M_PI_4 * 3, 0);
}

double GameEngine::getTrajectoryHeight(Player* p, double x){
    double t = (x - p->position) / (p->power * cos(p->angle));
    return t * (sin(p->angle) - g*t/2);
}

bool GameEngine::wallHit(Player* player){
    double left = getTrajectoryHeight(player, wall.left);
    double right = getTrajectoryHeight(player, wall.right);
    return (left <= wall.top || right <= wall.right);
}

double GameEngine::getImpactPosition(Player* player) {
    double dS = 2 * (player->power * MAX_V)*(player->power * MAX_V) * cos(player->angle) * sin(player->angle) / g;
    return player->position + dS;
 }

bool GameEngine::firePlayer(Player* source, Player* target) {
    if (!wallHit(source)) {
        double impactPosition = getImpactPosition(source);
        if (impactPosition-0.05 < target->position || impactPosition+0.05 > target->position) {     //Eltalálta
            target->power -= 10;
            return true;
        }
    }
    return false;
}

bool GameEngine::setLocalPlayer(double position, double angle, double power){
    if (isLocalTurn) {
        localPlayer.position = position;
        localPlayer.angle = angle;
        localPlayer.power = power;
        return true;
    } else {
        return false;
    }
}

bool GameEngine::setRemotePlayer(double position, double angle, double power) {
    if (!isLocalTurn) {
        remotePlayer.position = position;
        remotePlayer.angle = angle;
        remotePlayer.power = power;
        return false;
    } else {
        return true;
    }
}

int GameEngine::getLocalPlayerHp(){
    return localPlayer.hp;
}

int GameEngine::getRemotePlayerHp(){
    return remotePlayer.hp;
}

bool GameEngine::fireLocalPlayer(){
    return firePlayer(&localPlayer, &remotePlayer);
}

bool GameEngine::fireRemotePlayer(){
    return firePlayer(&remotePlayer, &localPlayer);
}

GameEngine::Position GameEngine::getBulletPosition(double deltaTime){
    //Nincs még implementálva
    Position position;
    return position;
}
