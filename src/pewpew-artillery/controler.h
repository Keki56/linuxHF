#ifndef CONTROLER_H
#define CONTROLER_H

#include <QObject>
#include "gameengine.h"

class Controler : public QObject
{
private:
    Q_OBJECT
    GameEngine engine;
public:
    explicit Controler(QObject *parent = 0);


signals:

public slots:



};

#endif // CONTROLER_H
