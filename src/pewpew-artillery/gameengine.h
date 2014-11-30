#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <qstring.h>

class GameEngine
{
private:
    struct Player {
        int hp;
        double position;
        double angle;
        double power;
    };

    struct Wall {
        double left, right, top;
    };

    Player localPlayer, remotePlayer;
    Wall wall;
    bool isLocalTurn;

    double getTrajectoryHeight(Player* p, double x);
    bool wallHit(Player* player);
    double getImpactPosition(Player* p);
    bool firePlayer(Player* source, Player* target);
public:
    struct Position {
        double x;
        double y;
    };

    GameEngine(/*bool localStarts*/);

    bool setLocalPlayer(double position, double angle, double power);
    bool setRemotePlayer(double position, double angle, double power);

    bool fireLocalPlayer();
    bool fireRemotePlayer();

    int getLocalPlayerHp();
    int getRemotePlayerHp();

    Position getBulletPosition(double deltaTime);
};

#endif // GAMEENGINE_H
