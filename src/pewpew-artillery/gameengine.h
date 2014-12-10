#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <qstring.h>
#include <QPointF>

//A pálya 0.0-tól 1.0-ig tart


#define WALL_LEFT 0.45              //a fal bal széle
#define WALL_RIGHT 0.55             //a fal jobb széle
#define WALL_TOP 0.25               //a fal magassága
#define DEFAULT_LEFT_POSITION 0.2   //a játékosok alapértelmezett pozíciója     |-x-P1-----WALL-----P2-x-|
#define DEFAULT_RIGHT_POSITION 0.8
#define RADIUS 0.15                  //mennyire távolodhatnak el a játékosok a kiindulópozíciójuktól
#define IMPACT_RADIUS 0.1            //a becsapódástól maximum mekkora távolságra sebez a lövedék
#define MAX_HP 100                   //maxminális, kezdeti életerő
#define MAX_DAMAGE 20                //maximális sebződés telitalálat esetén

class GameEngine
{
private:
    struct Player {
        double defaultPosition;
        double hp;
        double position;
        double angle;
        double power;
    };

    struct Wall {
        double left, right, top;
    };

    Player localPlayer, remotePlayer;
    Wall wall;
    bool isLocalTurn, isLocalLeft;

    double getTrajectoryHeight(Player* p, double x);
    double wallHit(Player* player);
    double getImpactTime(Player* p);
    double firePlayer(Player* source, Player* target);
    bool positionValidator(Player* player, double position) const;
    bool angleValidator(double angle) const;
    void damage(Player* player, double impactPosition);
public:
    GameEngine(bool isLocalStart, bool isLocalLeft);

    /**
     * @brief setLPPosition
     * @param position
     * @return true if @param is a valid position
     */
    bool setLocalPlayerPosition(double position);

    /**
     * @brief setLPAngle
     * @param angle
     * @return true if @param is a valid angle
     */
    bool setLocalPlayerAngle(double angle);

    /**
     * @brief setPower
     * @param power
     * @return true if @param is a valid power
     */
    bool setLocalPlayerPower(double power);

    bool setRemotePlayerPosition(double position);
    bool setRemotePlayerAngle(double angle);
    bool setRemotePlayerPower(double power);

    double fireLocalPlayer();
    double fireRemotePlayer();

    double getLocalPlayerPosition() const;
    double getLocalPlayerAngle() const;
    double getLocalPlayerPower() const;
    double getLocalPlayerHP() const;

    double getRemotePlayerPosition() const;
    double getRemotePlayerAngle() const;
    double getRemotePlayerHP() const;

    bool getLocalLeft() const;
    bool getLocalTurn() const;

    QPointF getBulletPosition(double deltaTime);
};

#endif // GAMEENGINE_H
