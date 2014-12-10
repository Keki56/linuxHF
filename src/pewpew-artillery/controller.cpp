#include "controller.h"
#include "gamewindow.h"
#include "lobby.h"
#include "messagetypes.h"
#include <stdio.h>
#include <QApplication>
#include <QMessageBox>
#include <QDebug>

#define STEP_SIZE 0.01
#define TURN_STEP 0.1256637     //PI / 25

Controller::Controller(bool localStarts, Lobby *parent) :
    QObject(parent),
    lobby(parent),
    engine(localStarts, localStarts),
    window(this, &engine),
    //isLocalTurn(localStarts),
    opponentName(""),
    bulletPosition(-1,-1)
{
    window.show();
    window.refresh();
    window.refreshHP();
}

void Controller::sendMoveMessage(double deltaHP){
    QuadDoubleMessage msg;
    msg.type = MSGT_PLAYER_MOVED;
    msg.data[0] = engine.getLocalPlayerPosition();
    msg.data[1] = engine.getLocalPlayerAngle();
    msg.data[2] = engine.getLocalPlayerPower();
    msg.data[3] = deltaHP;
    lobby->sendMessage(msg);
}

/**
 * @brief Checks both players are alive
 */
void Controller::checkPlayersAlive() {
    if (engine.getLocalPlayerHP() <= 0.0) {
        qDebug() << "Remote player win!";
    } else if (engine.getRemotePlayerHP() <= 0.0) {
        qDebug() << "Local player win!";
    }

}

/**
 * @brief Local player changes his position by one step.
 */
void Controller::onChangeLocalPosition(direction direction) {
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
void Controller::onChangeLocalAngle(direction direction) {
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
 * @brief The remote player changes his position during an animation.
 * @param position The position to set.
 */
void Controller::onChangeRemotePosition(double position) {
    if (engine.setRemotePlayerPosition(position)) {
        //megengedett elmozdulás
        window.refresh();
    } else {
        //nem megengedett elmozdulás (vagy nem is a távoli játékos jön)
        return;
    }
}

/**
 * @brief The remote player changes his cannon's angle during an animation.
 * @param angle The angle to set.
 */
void Controller::onChangeRemoteAngle(double angle) {
    if (engine.setRemotePlayerAngle(angle)){
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
void Controller::onChangeLocalPower(double power){
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
    qDebug() << "ontroller::fireLocalPlayer()";
    double oldHP = engine.getRemotePlayerHP();
    double bulletTime = engine.fireLocalPlayer();
    if (bulletTime >= 0) {
        //window.setFireEnabled(false);
        double deltaHP = oldHP - engine.getRemotePlayerHP();
        sendMoveMessage(deltaHP);
        checkPlayersAlive();
        if (animation != NULL) {
            printf("Controller::fireLocalPlayer - Az ellenfél lövésanimációja alatt már a helyi játékos tud mozgni és lőni. Ez nem ideális, de egyenlőre így van.\n");
            window.refresh();
        } else {
            animation = new Animation(this, bulletTime);
            animation->startAnimation();
            connect(animation, SIGNAL(animationFinished()), SLOT(fireAnimationFinnished()));
        }
    } else {
        printf("Controller:fireLocalPlayer - LocalPlayer tried to fire out of his turn.\n");
        qApp->exit(-1);
    }
}

void Controller::onMessageReceived(double position, double angle, double power, double deltaHP){
    qDebug() << "Controller::onMessageReceived";
    qDebug() << QString("Controller::onMessageReceived - position=%1 angle=%2 power=%3 deltaHP=%4").arg(position).arg(angle).arg(power).arg(deltaHP);
    if (animation != NULL) {
        printf("Controller::onMessageReceived - Message arrived while animating. It is imopossible, so there must be an error or a bug. Message is dropped\n");
        return;
    }
    this->deltaHP = deltaHP;
    engine.setRemotePlayerPower(power);
    animation = new Animation(this,
                              engine.getRemotePlayerPosition(), position,
                              engine.getRemotePlayerAngle(), angle);
    animation->startAnimation();
    connect(animation, SIGNAL(animationFinished()), SLOT(playerAnimationFinished()));
}

void Controller::onOpponentJoined(const QString& name) {
    printf("Controller::onOpponentJoined - name=%s\n", name.toLocal8Bit().data());
    opponentName = name;
    window.refresh();
}

void Controller::onOpponentQuit() {
    printf("Controller::onOpponentQuit\n"); //TODO törölni
    QMessageBox::information(&window, tr("PewPew Altillery"), opponentName.append(tr(" kilépett.")));
}

/**
 * @brief Controller::animateBullet
 * @param time
 */
void Controller::animateBullet(double deltaTime) {
    qDebug() << QString("Controller::animateBullet(%1)").arg(deltaTime);
    bulletPosition = engine.getBulletPosition(deltaTime);
    window.refresh();
}

/*void Controller::gameFinished(bool isLocalPlayerWin){
    if (isLocalPlayerWin)
        qDebug << "Local player win!";
    else
        qDebug << "Remote player win!";
}*/

void Controller::onWindowClosed() {
    lobby->gameClosed();
}

bool Controller::hasGameStarted() const {
    return !opponentName.isEmpty();
}

/**
 * @brief Get the name of the local player.
 * @return The name of the local player.
 */
QString Controller::getLocalPlayerName() const {
    return lobby->getPlayerName();
}

/**
 * @brief Get the name of the remote player.
 * @return The name of the remote player.
 */
QString Controller::getRemotePlayerName() const {
    return opponentName;
}

QPointF Controller::getBulletPosition() const{
    return bulletPosition;
}

/**
 * @brief Event handler after remote player animation has finished;
 */
void Controller::playerAnimationFinished() {
    if (animation == NULL) return;
    animation->deleteLater();
    animation = NULL;
    //QMessageBox::information(&window, "info", "The animation has finished.");
    qDebug() << "Controller::animationFinished - The animation has finished.";

    double localPlayerHPOld = engine.getLocalPlayerHP();
    qDebug() << "Controller - remote player fired";
    double bulletTime = engine.fireRemotePlayer();      //TODO a helyi játékos a lövés animációja alatt már tud mászkálni. Elugrani nem tud a lövedék elöl, mert a lövés már ekkora kifejtette hatását, ilyenkor már csak az animáció megy. (Figyelni a GameEngine::getBulletPosition-re, ha ezen javítanék.)
    //window.refresh();
    checkPlayersAlive();
    double deltaHPDifference = (localPlayerHPOld - engine.getLocalPlayerHP()) - deltaHP;
    if (deltaHPDifference > 0.1 || deltaHPDifference < -0.1) {
        printf("Controller::animationFinished - Difference between local and remote deltaHP is %f > 0.1", deltaHPDifference);
        qApp->exit(-1);
    }
    animation = new Animation(this, bulletTime);
    animation->startAnimation();
    connect(animation, SIGNAL(animationFinished()), SLOT(fireAnimationFinnished()));
}

void Controller::fireAnimationFinnished() {
    if (animation == NULL) return;
    animation->deleteLater();
    animation = NULL;
    bulletPosition = QPointF(-1, -1);
    window.refreshHP();
}
