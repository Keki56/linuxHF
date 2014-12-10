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
        isGameFinished = true;
        QMessageBox::information(&window, tr("PewPew Altillery"), tr("Vesztettél!"));
    } else if (engine.getRemotePlayerHP() <= 0.0) {
        isGameFinished = true;
        QMessageBox::information(&window, tr("PewPew Altillery"), tr("Győztél!"));
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
    double oldHP = engine.getRemotePlayerHP();
    double bulletTime = engine.fireLocalPlayer();
    if (bulletTime >= 0) {
        //window.setFireEnabled(false);
        double deltaHP = oldHP - engine.getRemotePlayerHP();
        sendMoveMessage(deltaHP);
        if (animation != NULL) {
            window.refresh();
            checkPlayersAlive();
        } else {
            animation = new Animation(this, bulletTime);
            animation->startAnimation();
            connect(animation, SIGNAL(animationFinished()), SLOT(fireAnimationFinnished()));
        }
    } else {
        qApp->exit(-1);
    }
}

void Controller::onMessageReceived(double position, double angle, double power, double deltaHP){
    if (animation != NULL) return;
    this->deltaHP = deltaHP;
    engine.setRemotePlayerPower(power);
    animation = new Animation(this,
                              engine.getRemotePlayerPosition(), position,
                              engine.getRemotePlayerAngle(), angle);
    animation->startAnimation();
    connect(animation, SIGNAL(animationFinished()), SLOT(playerAnimationFinished()));
}

void Controller::onOpponentJoined(const QString& name) {
    opponentName = name;
    window.refresh();
}

void Controller::onOpponentQuit() {
    QMessageBox::information(&window, tr("PewPew Altillery"), opponentName.append(tr(" kilépett.")));
}

/**
 * @brief Controller::animateBullet
 * @param time
 */
void Controller::animateBullet(double deltaTime) {
    bulletPosition = engine.getBulletPosition(deltaTime);
    window.refresh();
}

/**
 * @brief Event handler after the window was closed.
 */
void Controller::onWindowClosed() {
    lobby->gameClosed();
}

/**
 * @brief Chech whether the game has started.
 * @return True if the game has started.
 */
bool Controller::hasGameStarted() const {
    return !opponentName.isEmpty();
}

bool Controller::hasGameFinished() const {
    return isGameFinished;
}

/**
 * @brief Check whether an animation is running.
 * @return True if an animation is running.
 */
bool Controller::isAnimationRunning() const {
    return (animation != NULL);
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

    double localPlayerHPOld = engine.getLocalPlayerHP();
    double bulletTime = engine.fireRemotePlayer();      //TODO a helyi játékos a lövés animációja alatt már tud mászkálni. Elugrani nem tud a lövedék elöl, mert a lövés már ekkora kifejtette hatását, ilyenkor már csak az animáció megy. (Figyelni a GameEngine::getBulletPosition-re, ha ezen javítanék.)
    double deltaHPDifference = (localPlayerHPOld - engine.getLocalPlayerHP()) - deltaHP;
    if (deltaHPDifference > 0.1 || deltaHPDifference < -0.1) {
        qApp->exit(-1);
    }
    animation = new Animation(this, bulletTime);
    animation->startAnimation();
    connect(animation, SIGNAL(animationFinished()), SLOT(fireAnimationFinnished()));
    //checkPlayersAlive();
}

void Controller::fireAnimationFinnished() {
    if (animation == NULL) return;
    animation->deleteLater();
    animation = NULL;
    bulletPosition = QPointF(-1, -1);
    window.refreshHP();
    checkPlayersAlive();
}
