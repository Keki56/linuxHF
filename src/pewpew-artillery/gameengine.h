#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <qstring.h>
#include <QPointF>

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
    bool wallHit(Player* player);
    double getImpactPosition(Player* p);
    bool firePlayer(Player* source, Player* target);
    bool positionValidator(Player* player, double position) const;
    bool angleValidator(double angle) const;
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

    bool fireLocalPlayer();
    //bool fireRemotePlayer(double position, double angle, double power, double deltaHP);
    bool fireRemotePlayer();

    double getLocalPlayerPosition() const;
    double getLocalPlayerAngle() const;
    double getLocalPlayerHP() const;

    double getRemotePlayerPosition() const;
    double getRemotePlayerAngle() const;
    double getRemotePlayerHP() const;

    bool getLocalLeft() const;
    bool getLocalTurn() const;

    QPointF getBulletPosition(double deltaTime);
};

#endif // GAMEENGINE_H
