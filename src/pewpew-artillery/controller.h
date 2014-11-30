#ifndef CONTROLER_H
#define CONTROLER_H

#include <QObject>
#include "gameengine.h"
#include "gamewindow.h"

class Controller : public QObject
{
private:
    Q_OBJECT
    GameWindow window;
    GameEngine engine;
    bool isLocalTurn;
public:
    explicit Controller(bool localStarts, QObject *parent = 0);

    bool setRemotePlayer(double position, double angle, double power);
    bool fireRemotePlayer();
    bool fireLocalPlayer();

signals:

public slots:



};

#endif // CONTROLER_H
