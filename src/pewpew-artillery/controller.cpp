#include "controller.h"
#include "gamewindow.h"
#include "lobby.h"
#include <stdio.h>

Controller::Controller(bool localStarts, Lobby *parent) :
    isLocalTurn(localStarts),
    QObject(parent),
    lobby(parent),
    window(this),
    engine(localStarts)
{
    window.show();
}

bool Controller::fireRemotePlayer(){
    printf("Controller::fireRemotePlayer\n");
}

bool Controller::fireLocalPlayer(){
    printf("Controller::fireLocaPlayer\n");
}

void Controller::onMessageReceived(double position, double angle, double power, double deltaHP){
    printf("Controller::onMessageReceived - position=%f angle=%f power=%f deltaHP=%f\n", position, angle, power, deltaHP);
}

void Controller::onOpponentJoined(const QString& name) {
    printf("Controller::onOpponentJoined - name=%s\n", name.toLocal8Bit().data());
}

void Controller::onOpponentQuit() {
    printf("Controller::onOpponentQuit\n");
}

void Controller::onWindowClosed() {
    printf("Controller::onWindowClosed\n");
}
