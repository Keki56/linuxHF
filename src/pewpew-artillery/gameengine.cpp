#include "gameengine.h"
#include <cmath>
#include <stdio.h>

//A pálya 0.0-tól 1.0-ig tart

#define g 9.81                      //a gracitációs gyorsulás
#define MAX_V 100                   //a lövedék maximális kezdősebessége

#define WALL_LEFT 0.45              //a fal bal széle
#define WALL_RIGHT 0.55             //a fal jobb széle
#define WALL_TOP 0.00001                //a fal magassága
#define DEFAULT_LEFT_POSITION 0.2   //a játékosok alapértelmezett pozíciója     |-x-P1-----WALL-----P2-x-|
#define DEFAULT_RIGHT_POSITION 0.8
#define RADIUS 0.04                  //mennyir távolodhatnak el a játékosok a kiindulópozíciójuktól
#ifndef M_PI_4
#define M_PI_4		0.78539816339744830962
#endif
#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

GameEngine::GameEngine(bool isLocalStart, bool isLocalLeft):
    isLocalTurn(isLocalStart),
    isLocalLeft(isLocalLeft)
{
    //init wall
    wall.left = WALL_LEFT;
    wall.right = WALL_RIGHT;
    wall.top = WALL_TOP;

    //init players
    if(isLocalLeft) {
        localPlayer.position = localPlayer.defaultPosition = DEFAULT_LEFT_POSITION;
        localPlayer.angle = M_PI_4;
        remotePlayer.position = remotePlayer.defaultPosition = DEFAULT_RIGHT_POSITION;
        remotePlayer.angle = M_PI_4 * 3;
    } else {
        localPlayer.position = localPlayer.defaultPosition = DEFAULT_RIGHT_POSITION;
        localPlayer.angle = M_PI_4 * 3;
        remotePlayer.position = remotePlayer.defaultPosition = DEFAULT_LEFT_POSITION;
        remotePlayer.angle = M_PI_4;
    }
    localPlayer.hp = remotePlayer.hp = 100;
}

double GameEngine::getTrajectoryHeight(Player* p, double x){
    double t = (x - p->position) / (p->power * std::cos(p->angle));
    return t * (std::sin(p->angle) - g*t/2);
}

bool GameEngine::wallHit(Player* player){
    double left = getTrajectoryHeight(player, wall.left);
    double right = getTrajectoryHeight(player, wall.right);
    return (left <= wall.top || right <= wall.right);
}

double GameEngine::getImpactPosition(Player* player) {
    double dS = 2 * (player->power * MAX_V)*(player->power * MAX_V) * std::cos(player->angle) * std::sin(player->angle) / g;
    return player->position + dS;
 }

bool GameEngine::firePlayer(Player* source, Player* target) {
    if (!wallHit(source)) {
        double impactPosition = getImpactPosition(source);
        printf("GameEngine::firePlayer - impactPosition=%f target->position=%f\n", impactPosition, target->position);         //TODO törölni
        if (impactPosition-0.05 < target->position || impactPosition+0.05 > target->position) {     //Eltalálta
            target->power -= 10;
            return true;
        }
    }
    printf("GameEngine::firePlayer - falat ért\n");
    return false;
}

bool GameEngine::positionValidator(Player *player, double position) const{
    return std::abs(position - player->defaultPosition) < RADIUS ? true : false;
}

bool GameEngine::angleValidator(double angle) const {
    return (angle > 0.0 && angle < M_PI);
}

//------------------------------------

bool GameEngine::fireRemotePlayer(double position, double angle, double power, double deltaHP){
    if (!isLocalTurn && positionValidator(&remotePlayer, position) && angleValidator(angle)) {      //Elvileg nem érkezhetne abnormális adat.
        remotePlayer.position = position;
        remotePlayer.angle = angle;
        remotePlayer.power = power;

        firePlayer(&remotePlayer, &localPlayer);
        isLocalTurn = true;

        return true;
    } else {
        return false;
    }
}

bool GameEngine::setLocalPlayerPosition(double position) {
    if (isLocalTurn && positionValidator(&localPlayer, position)) {
        localPlayer.position = position;
        printf("Position=%f\n", position);
        return true;
    } else {
        return false;
    }
}

bool GameEngine::setLocalPlayerAngle(double angle) {
    if (isLocalTurn && angleValidator(angle)) {
        localPlayer.angle = angle;
        return true;
    } else {
        return false;
    }
}

bool GameEngine::setLocalPlayerPower(double power){
    if (isLocalTurn) {
        localPlayer.power = power;
        return true;
    } else {
        return false;
    }
}

bool GameEngine::setRemotePlayerPosition(double position) {
    if (!isLocalTurn && positionValidator(&remotePlayer, position)) {
        remotePlayer.position = position;
        printf("Position=%f\n", position);
        return true;
    } else {
        return false;
    }
}

bool GameEngine::setRemotePlayerAngle(double angle) {
    if (!isLocalTurn && angleValidator(angle)) {
        remotePlayer.angle = angle;
        return true;
    } else {
        return false;
    }
}

bool GameEngine::setRemotePlayerPower(double power) {
    if (!isLocalTurn) {
        remotePlayer.power = power;
        return true;
    } else {
        return false;
    }
}

bool GameEngine::fireLocalPlayer(){
    if (isLocalTurn) {
        firePlayer(&localPlayer, &remotePlayer);
        isLocalTurn = false;
        return true;
    } else {
        return false;
    }
}

double GameEngine::getLocalPlayerPosition() const{
    return localPlayer.position;
}

double GameEngine::getLocalPlayerAngle() const{
    return localPlayer.angle;
}

double GameEngine::getLocalPlayerHP() const{
    return localPlayer.hp;
}

double GameEngine::getRemotePlayerPosition() const{
    return remotePlayer.position;
}

double GameEngine::getRemotePlayerAngle() const{
    return remotePlayer.angle;
}

double GameEngine::getRemotePlayerHP() const{
    return remotePlayer.hp;
}

bool GameEngine::getLocalLeft() const{
    return isLocalLeft;
}

bool GameEngine::getLocalTurn() const {
    return isLocalTurn;
}


QPointF GameEngine::getBulletPosition(double deltaTime){
    Player* player;
    if (isLocalTurn)
        player = &remotePlayer;
    else
        player = &localPlayer;

    double x = player->power * std::cos(player->angle) * deltaTime;
    double y = player->power * std::sin(player->angle) * deltaTime - g / 2 * deltaTime * deltaTime;
    QPointF position(x,y);
    return position;
}
