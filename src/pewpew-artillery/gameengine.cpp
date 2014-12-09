#include "gameengine.h"
#include <cmath>
#include <stdio.h>

#include <QDebug>

//A pálya 0.0-tól 1.0-ig tart

#define g 9.81                      //a gracitációs gyorsulás
//#define MAX_V 100                   //a lövedék maximális kezdősebessége

#define WALL_LEFT 0.45              //a fal bal széle
#define WALL_RIGHT 0.55             //a fal jobb széle
#define WALL_TOP 0.01               //a fal magassága
#define DEFAULT_LEFT_POSITION 0.2   //a játékosok alapértelmezett pozíciója     |-x-P1-----WALL-----P2-x-|
#define DEFAULT_RIGHT_POSITION 0.8
#define RADIUS 0.15                  //mennyire távolodhatnak el a játékosok a kiindulópozíciójuktól
#define IMPACT_RADIUS 1              //a becsapódástól maximum mekkora távolságra sebez a lövedék
#define MAX_HP 100                   //maxminális, kezdeti életerő
#define MAX_DAMAGE 20                //maximális sebződés telitalálat esetén
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
    localPlayer.hp = remotePlayer.hp = MAX_HP;
}

double GameEngine::getTrajectoryHeight(Player* p, double x){
    //algo No.1
    double t = std::abs(x - p->position) / (p->power * std::cos(p->angle));
    double result1 = t * (p->power * std::sin(p->angle) - g*t/2);
    //algo No.2
    double s = std::abs(x - p->position);
    double result2 = s * (std::sin(p->angle) - g * s / (2 * p->power * p->power));
    qDebug() << QString("1 t=%1 result1=%2").arg(t).arg(result1);
    qDebug() << QString("2 s=%1 resutl2=%2").arg(s).arg(result2);


    return result2;
}

bool GameEngine::wallHit(Player* player){
    qDebug("Wall.left");
    double left = getTrajectoryHeight(player, wall.left);
    qDebug("Wall.eight");
    double right = getTrajectoryHeight(player, wall.right);
    return (left <= wall.top || right <= wall.top);
}

double GameEngine::getImpactPosition(Player* player) {
    double dS = 2 * player->power*player->power * std::cos(player->angle) * std::sin(player->angle) / g;
    return player->position + dS;
 }

void GameEngine::damage(Player *player, double impactPosition){
    double impDist = std::abs(player->position - impactPosition);
    if (impDist < IMPACT_RADIUS) {
        player->hp -= MAX_DAMAGE - MAX_DAMAGE * (impDist / IMPACT_RADIUS);
    }
}

bool GameEngine::firePlayer(Player* source, Player* target) {
    qDebug() << QString("position=%1\nangle=%2\npower=%3\n").arg(source->position).arg(source->angle).arg(source->power);
    if (!wallHit(source)) {
        double impactPosition = getImpactPosition(source);
        qDebug() << QString("impactPosition=%1").arg(impactPosition);
        damage(source, impactPosition);     //Magunkba is belelőhetunk
        damage(target, impactPosition);
        return true;
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
        printf("GameEngine::fireLocalPlayer");

        //DEBUG
        //localPlayer.position = 0.2;
        //localPlayer.angle = 0.7854;
        //localPlayer.power = 2.426;
        //DEBUG

        firePlayer(&localPlayer, &remotePlayer);
        isLocalTurn = false;
        return true;
    } else {
        return false;
    }
}

bool GameEngine::fireRemotePlayer(){
    if (!isLocalTurn) {
        firePlayer(&remotePlayer, &localPlayer);
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

/**
 * @brief Get the position of the last fired bullet
 * @param deltaTime
 * @return bullet position in 2D
 */
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
