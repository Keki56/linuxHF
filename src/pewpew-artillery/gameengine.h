#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <qstring.h>

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
    struct Position {
        double x;
        double y;
    };

    GameEngine(bool isLocalStart, bool isLocalLeft);

    bool setLocalPlayer(double position, double angle, double power);       //TODO törölni.
    bool fireRemotePlayer(double position, double angle, double power, double deltaHP);

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

    bool fireLocalPlayer();

    int getLocalPlayerHp();
    int getRemotePlayerHp();

    Position getBulletPosition(double deltaTime);
};

#endif // GAMEENGINE_H
