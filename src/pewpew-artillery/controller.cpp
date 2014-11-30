#include "controller.h"
#include "gamewindow.h"
#include <stdio.h>

Controller::Controller(bool localStarts, QObject *parent) :
    QObject(parent),
    isLocalTurn(localStarts),
    window(this)
{
    window.show();
}

 bool Controller::fireLocalPlayer(){
     printf("Controller::fireLocaPlayer\n");
 }

