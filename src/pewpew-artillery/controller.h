#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QString>
#include "gameengine.h"
#include "gamewindow.h"
class Lobby;


class Controller : public QObject
{
    Q_OBJECT

private:  
    Lobby* lobby;
    GameEngine engine;
    GameWindow window;
    bool isLocalTurn;
    QString opponentName;
public:
    enum direction {LEFTtoRIGHT = 1, RIGHTtoLEFT = -1,
                    COUNTERCLOCKWISE = 1, CLOCKWISE = -1};

    explicit Controller(bool localStarts, Lobby *parent = 0);

    //bool setRemotePlayer(double position, double angle, double power);
    bool fireRemotePlayer();        //csak debug célra!!!
    bool fireLocalPlayer();

    void onChangePosition(direction direction);
    void onChangeAngle(direction direction);
    void onChangePower(double power);
    void onSendChat(const QString& message);
    void onReceiveChat(const QString& message);

    void onMessageReceived(double position, double angle, double power, double deltaHP);
    void onOpponentJoined(const QString& name);
    void onOpponentQuit();
    void onWindowClosed();

    bool hasGameStarted() const;

signals:

public slots:



};

#endif // CONTROLLER_H
