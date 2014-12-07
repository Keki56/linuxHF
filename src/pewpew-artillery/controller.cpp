#include "controller.h"
#include "gamewindow.h"
#include "lobby.h"
#include <stdio.h>
#include <QApplication>
#include <QMessageBox>

#define STEP_SIZE 0.01
#define TURN_STEP 0.1256637     //PI / 25

Controller::Controller(bool localStarts, Lobby *parent) :
    QObject(parent),
    lobby(parent),
    engine(localStarts, true),
    window(this, &engine),
    isLocalTurn(localStarts),
    opponentName("")

{
    window.show();
    window.refresh();
}

/*bool Controller::fireRemotePlayer(){
    printf("Controller::fireRemotePlayer\n");
    return true;
}*/

bool Controller::fireLocalPlayer(){
    printf("Controller::fireLocaPlayer\n");
    return true;
}

/**
 * @brief Local player changes his position by one step.
 */
void Controller::onChangePosition(direction direction) {
    double newPosition = engine.getLocalPlayerPosition() + direction*STEP_SIZE;
    if (engine.setLocalPlayerPosition(newPosition)) {
        //megengedett elmozdulás
    } else {
        //nem megengedett elmozdulás (vagy nem is a helyi játékos jön)
    }
}

/**
 * @brief Local player changes his cannon's angle by one step
 */
void Controller::onChangeAngle(direction direction) {
    double newAngle = engine.getLocalPlayerAngle() + direction*TURN_STEP;
    if (engine.setLocalPlayerAngle(newAngle)){
        //megengedett ágyúállás
        int value = newAngle * 31.51268;
        window.setSliderValue(value);
    } else {
        //nem megengedett
    }
}

/**
 * @brief Local player changes his fire power
 */
void Controller::onChangePower(double power){
    if (engine.setLocalPlayerPower(power)) {
        //megengedett állítás
    } else {
        //nem megengedett
    }
}

void Controller::onMessageReceived(double position, double angle, double power, double deltaHP){
    printf("Controller::onMessageReceived - position=%f angle=%f power=%f deltaHP=%f\n", position, angle, power, deltaHP);

    if (engine.fireRemotePlayer(position, angle, power, deltaHP)) {
        window.setEnabled(true);
    } else {
        printf("Controller:onMessageReveiced - Incorrect data error.\n");
        qApp->exit(-1);
    }
}

void Controller::onOpponentJoined(const QString& name) {
    printf("Controller::onOpponentJoined - name=%s\n", name.toLocal8Bit().data());
    opponentName = name;
}

void Controller::onOpponentQuit() {
    printf("Controller::onOpponentQuit\n");
}

void Controller::onWindowClosed() {
    lobby->gameClosed();
}

bool Controller::hasGameStarted() const {
    return !opponentName.isEmpty();
}
