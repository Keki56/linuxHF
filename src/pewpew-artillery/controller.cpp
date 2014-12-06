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

