#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QString>
#include <QPointF>
#include "gameengine.h"
#include "gamewindow.h"
#include "animation.h"
class Lobby;

class Controller : public QObject
{
    Q_OBJECT

private:  
    Lobby* lobby;
    GameEngine engine;
    GameWindow window;
    //bool isLocalTurn;
    QString opponentName;
    Animation* animation = NULL;
    QPointF bulletPosition;
    double deltaHP;

    void sendMoveMessage(double deltaHP);
    void checkPlayersAlive();
public:
    enum direction {LEFTtoRIGHT = 1, RIGHTtoLEFT = -1,
                    COUNTERCLOCKWISE = 1, CLOCKWISE = -1};

    explicit Controller(bool localStarts, Lobby *parent = 0);

    void onChangeLocalPosition(direction direction);
    void onChangeLocalAngle(direction direction);
    void onChangeLocalPower(double power);
    void fireLocalPlayer();

    void onChangeRemotePosition(double position);
    void onChangeRemoteAngle(double angle);

    void onMessageReceived(double position, double angle, double power, double deltaHP);
    void onOpponentJoined(const QString& name);
    void onOpponentQuit();

    void animateBullet(double deltaTime);

    //void gameFinished(bool isLocalPlayerWin);

    void onSendChat(const QString& message);
    void onReceiveChat(const QString& message);

    void onSendChat();
    void onWindowClosed();

    bool hasGameStarted() const;
    QString getLocalPlayerName() const;
    QString getRemotePlayerName() const;
    QPointF getBulletPosition() const;


    /* TEMP */ void testAnimation();

signals:

public slots:
    void playerAnimationFinished();
    void fireAnimationFinnished();

};

#endif // CONTROLLER_H
