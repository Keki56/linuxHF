#include "gameengine.h"
#include <cmath>
#include <stdio.h>

#define g 9.81                      //a gracitációs gyorsulás
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
    // double t = std::abs(x - p->position) / (p->power * std::cos(p->angle));
    // double result1 = t * (p->power * std::sin(p->angle) - g*t/2);
    //algo No.2
    double s = std::abs(x - p->position);
    double result2 = s * (std::sin(p->angle) - g * s / (2 * p->power * p->power));

    return result2;
}

/**
 * @brief Calculate when the bullet hits the wall
 * @return The time when the bullet hits the wall or -1, if not.
 */

double GameEngine::wallHit(Player* player){
    double leftH = getTrajectoryHeight(player, wall.left);
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
    double wallHitTime = wallHit(source);
    if (wallHitTime < 0) {
        double impactTime = getImpactTime(source);
        double impactPosition = source->position + impactTime * source->power * std::cos(source->angle);
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

double GameEngine::fireLocalPlayer(){
    if (isLocalTurn) {
        isLocalTurn = false;
        return firePlayer(&localPlayer, &remotePlayer);
    } else {
        return -1;
    }
}

double GameEngine::fireRemotePlayer(){
    if (!isLocalTurn) {
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

    double x = player->position + player->power * std::cos(player->angle) * deltaTime;
    double y = player->power * std::sin(player->angle) * deltaTime - g / 2 * deltaTime * deltaTime;
    QPointF position(x,y);
    return position;
}
