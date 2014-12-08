#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QString>
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
    double deltaHP;

    void sendMoveMessage(double deltaHP);
public:
    enum direction {LEFTtoRIGHT = 1, RIGHTtoLEFT = -1,
                    COUNTERCLOCKWISE = 1, CLOCKWISE = -1};

    explicit Controller(bool localStarts, Lobby *parent = 0);

    void onChangePosition(direction direction);
    void onChangeAngle(direction direction);
    void onChangeRemotePosition(double position);
    void onChangeRemoteAngle(double angle);
    void onChangePower(double power);
    void onSendChat(const QString& message);
    void onReceiveChat(const QString& message);
    void fireLocalPlayer();

    void onMessageReceived(double position, double angle, double power, double deltaHP);
    void onOpponentJoined(const QString& name);
    void onOpponentQuit();

    void onSendChat();
    void onWindowClosed();

    bool hasGameStarted() const;
    QString getLocalPlayerName() const;
    QString getRemotePlayerName() const;

    /* TEMP */ void testAnimation();

signals:

public slots:
    void animationFinished();

};

#endif // CONTROLLER_H
