#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <qstring.h>
#include "player.h"
#include "map.h"

class GameEngine
{
private:
    Player localPlayer, remotePlayer;
    Map map;

    double getTrajectoryHeight(double x);
public:
    GameEngine();

    void setLocalPlayer(double position, double angle, double power);
    void setRemotePlayer(double position, double angle, double power);

    bool fireLocalPlayer();
    bool fireRemotePlayer();

    int getLocalPlayerHp();
    int getRemotePlayerHp();
};

#endif // GAMEENGINE_H
