#include "controller.h"
#include "gamewindow.h"
#include "lobby.h"
#include "messagetypes.h"
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
    window.showWaitingForOpponentDialog();
}

void Controller::sendMoveMessage(double deltaHP){
    QuadDoubleMessage msg;
    msg.type = MSGT_PLAYER_MOVED;
    msg.data[0] = engine.getLocalPlayerPosition();
    msg.data[1] = engine.getLocalPlayerAngle();
    msg.data[2] = engine.getLocalPlayerHP();
    msg.data[3] = deltaHP;
    lobby->sendMessage(msg);
}

/**
 * @brief Local player changes his position by one step.
 */
void Controller::onChangePosition(direction direction) {
    double newPosition = engine.getLocalPlayerPosition() + direction*STEP_SIZE;
    if (engine.setLocalPlayerPosition(newPosition)) {
        //megengedett elmozdulás
        window.refresh();
    } else {
        //nem megengedett elmozdulás (vagy nem is a helyi játékos jön)
        return;
    }
}

/**
 * @brief Local player changes his cannon's angle by one step
 */
void Controller::onChangeAngle(direction direction) {
    double newAngle = engine.getLocalPlayerAngle() + direction*TURN_STEP;
    if (engine.setLocalPlayerAngle(newAngle)){
        //megengedett ágyúállás        
        window.refresh();
    } else {
        //nem megengedett
        return;
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

/**
 * @brief Send a message through the in-game chat.
 * @param message The message to be sent.
 */
void Controller::onSendChat(const QString& message) {
    StringMessage msg;
    msg.type = MSGT_INGAME_CHAT_MESSAGE;
    msg.str = QString("%1: %2").arg(lobby->getPlayerName(), message);
    window.printChat(msg.str);
    lobby->sendMessage(msg);
}

/**
 * @brief Receive a chat message.
 * @param message The message received.
 */
void Controller::onReceiveChat(const QString& message) {
    window.printChat(message);
}

void Controller::fireLocalPlayer(){
    printf("Controller::fireLocaPlayer\n");

    double oldHP = engine.getLocalPlayerHP();
    if (engine.fireLocalPlayer()) {
        window.setFireEnabled(false);
        double deltaHP = oldHP - engine.getLocalPlayerHP();
        sendMoveMessage(deltaHP);
    } else {
        printf("Controller:fireLocalPlayer - LocalPlayer tried to fire out of his turn.\n");
        qApp->exit(-1);
    }
}

void Controller::onMessageReceived(double position, double angle, double power, double deltaHP){
    printf("Controller::onMessageReceived - position=%f angle=%f power=%f deltaHP=%f\n", position, angle, power, deltaHP);

    if (engine.fireRemotePlayer(position, angle, power, deltaHP)) {
        window.setFireEnabled(true);
    } else {
        printf("Controller:onMessageReveiced - Incorrect data error.\n");
        qApp->exit(-1);
    }
}

void Controller::onOpponentJoined(const QString& name) {
    printf("Controller::onOpponentJoined - name=%s\n", name.toLocal8Bit().data());
    opponentName = name;
    window.refresh();
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
