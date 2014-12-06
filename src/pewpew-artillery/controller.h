#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include "gameengine.h"
#include "gamewindow.h"
class Lobby;


class Controller : public QObject
{
private:
    Q_OBJECT
    Lobby* lobby;
    GameWindow window;
    GameEngine engine;
    bool isLocalTurn;
public:
    explicit Controller(bool localStarts, Lobby *parent = 0);

    bool setRemotePlayer(double position, double angle, double power);
    bool fireRemotePlayer();
    bool fireLocalPlayer();

    void onMessageReceived(double position, double angle, double power, double deltaHP);

signals:

public slots:



};

#endif // CONTROLLER_H
