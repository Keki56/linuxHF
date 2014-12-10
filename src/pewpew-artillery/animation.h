#ifndef ANIMATION_H
#define ANIMATION_H

#include <QObject>
#include <QTime>
class Controller;

enum AnimationState {
    ANST_POSITION = 0,
    ANST_ANGLE = 1,
    ANST_SHOT = 2
};

/**
 * @brief A class for handling animations for the remote player.
 */
class Animation : public QObject
{
Q_OBJECT
private:
    Controller* controller;
    AnimationState animState;

    double startPos, endPos;
    double startAngle, endAngle;
    int timerID;
    int moveDir, angleDir;

    double time;

    QTime startTime;

public:
    Animation(Controller* parent, double startPos, double endPos, double startAngle, double endAngle);
    Animation(Controller* parent, double time);

    void startAnimation();

protected:
    virtual void timerEvent(QTimerEvent*);

signals:
    void animationFinished();

public slots:

};

#endif // ANIMATION_H
