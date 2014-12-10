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
#define IMPACT_RADIUS 0.1            //a becsapódástól maximum mekkora távolságra sebez a lövedék
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

/**
 * @brief Calculate when the bullet hits the wall
 * @return The time when the bullet hits the wall or -1, if not.
 */
bool GameEngine::wallHit(Player* player){
    qDebug("Wall.left");
    double leftH = getTrajectoryHeight(player, wall.left);
    qDebug("Wall.eight");
    double rightH = getTrajectoryHeight(player, wall.right);
    double crashTime = -1;
    if (leftH <= wall.top){
        crashTime = (wall.left - player->position) / (player->power * std::cos(player->angle));
    }
    if (rightH <= wall.top){
        double crashTimeR = (wall.right - player->position) / (player->power * std::cos(player->angle));
        if (crashTime == -1) {
            crashTime = crashTimeR;
        } else if (crashTimeR < crashTime) {
            crashTime = crashTimeR;
        }
    }
    return crashTime;
}

double GameEngine::getImpactTime(Player* player) {
    double dT = 2 * player->power * std::sin(player->angle) / g;
    return dT;
 }

void GameEngine::damage(Player *player, double impactPosition){
    double impDist = std::abs(player->position - impactPosition);
    if (impDist < IMPACT_RADIUS) {
        player->hp -= MAX_DAMAGE - MAX_DAMAGE * (impDist / IMPACT_RADIUS);
    }
}

/**
 * @brief Perform a firing.
 * @param source - This player fires.
 * @param target - This player is the enemy.
 * @return - the time it takes the bullet strikes
 */
double GameEngine::firePlayer(Player* source, Player* target) {
    qDebug() << QString("position=%1\nangle=%2\npower=%3\n").arg(source->position).arg(source->angle).arg(source->power);
    double wallHitTime = wallHit(source);
    if (wallHitTime > 0) {
        double impactTime = getImpactTime(source);
        double impactPosition = source->position + impactTime * source->power * std::cos(source->angle);
        qDebug() << QString("impactPosition=%1").arg(impactPosition);
        damage(source, impactPosition);     //Magunkba is belelőhetunk
        damage(target, impactPosition);
        return impactTime;
    } else {
        return wallHitTime;
    }
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
        //printf("Position=%f\n", position);      //TODO törölni
        return true;
    } else {
        return false;
    }
}

bool GameEngine::setRemotePlayerAngle(double angle) {
    if (!isLocalTurn && angleValidator(angle)) {
        remotePlayer.angle = angle;
        //printf("Angle=%f\n", angle);             //TODO törölni
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

double GameEngine::fireLocalPlayer(){
    if (isLocalTurn) {
        printf("GameEngine::fireLocalPlayer");

        //DEBUG
        //localPlayer.position = 0.2;
        //localPlayer.angle = 0.7854;
        //localPlayer.power = 2.426;
        //DEBUG

        isLocalTurn = false;
        return firePlayer(&localPlayer, &remotePlayer);
    } else {
        return -1;
    }
}

double GameEngine::fireRemotePlayer(){
    if (!isLocalTurn) {
        qDebug() << "GameEngine::fireRemotePlayer()";
        isLocalTurn = true;
        return firePlayer(&remotePlayer, &localPlayer);
    } else {
        return -1;
    }
}

double GameEngine::getLocalPlayerPosition() const{
    return localPlayer.position;
}

double GameEngine::getLocalPlayerAngle() const{
    return localPlayer.angle;
}

double GameEngine::getLocalPlayerPower() const{
    return localPlayer.power;
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
