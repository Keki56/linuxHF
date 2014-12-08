#include "animation.h"
#include "controller.h"
#include <QTextStream>

#define STEP_SIZE 0.01
#define TURN_STEP 0.1256637     //PI / 25
#define TIMER_INTERVAL 20

/**
 * @brief Constructor for animating the remote player's move.
 * @param parent The controller instance.
 * @param startPos The starting position of the remote player.
 * @param endPos The desired position of the remote player.
 * @param startAngle The starting angle of the remote player.
 * @param endAngle The desired angle of the remote player.
 */
Animation::Animation(Controller *parent, double startPos, double endPos, double startAngle, double endAngle) :
    QObject(parent),
    controller(parent),
    animState(ANST_POSITION),
    startPos(startPos), endPos(endPos),
    startAngle(startAngle), endAngle(endAngle)
{
    moveDir = (endPos > startPos) ? 1 : -1;
    angleDir = (endAngle > startAngle) ? 1 : -1;
}

/**
 * @brief Contructor for animating a shot.
 * @param parent The controller instance.
 */
Animation::Animation(Controller* parent) :
    QObject(parent),
    controller(parent),
    animState(ANST_SHOT)
{

}

/**
 * @brief Start the animation.
 */
void Animation::startAnimation() {
    timerID = startTimer(TIMER_INTERVAL);
    startTime = QTime::currentTime();
}

/**
 * @brief Timer event for handling animations.
 */
void Animation::timerEvent(QTimerEvent*) {
    int elapsed = startTime.msecsTo(QTime::currentTime());
    switch (animState) {
        case ANST_POSITION: {
             double pos = startPos + (elapsed / 1000.0) * (endPos - startPos);
             if ((moveDir > 0) ^ (pos < endPos)) {
                animState = ANST_ANGLE;
                startTime = QTime::currentTime();
             }
             controller->onChangeRemotePosition(pos);
        break; }
        case ANST_ANGLE: {
            double angle = startAngle + (elapsed / 1000.0) * (endAngle - startAngle);
             if ((angleDir > 0) ^ (angle < endAngle)) {
                animState = ANST_SHOT;
                startTime = QTime::currentTime();
             }
             controller->onChangeRemoteAngle(angle);
        break; }
        case ANST_SHOT: {
            killTimer(timerID);
            emit animationFinished();
        break; }
    }
}
