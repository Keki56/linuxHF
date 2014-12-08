#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include "gameengine.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

class Controller;

namespace Ui {
class GameWindow;
}

class GameWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::GameWindow *ui;
    Controller* controller;
    GameEngine* engine;
    QGraphicsScene scene;
    QGraphicsPixmapItem* tankLeft, *tankRight, *turretLeft, *turretRight;

protected:
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void closeEvent(QCloseEvent* event);
    virtual void resizeEvent(QResizeEvent* event);
    virtual void showEvent(QShowEvent* event);

public:
    explicit GameWindow(Controller* controller, GameEngine* engine, QWidget *parent = 0);
    ~GameWindow();

    void refresh();
    void printChat(const QString& line);
    void setFireEnabled(bool enabled);//Balázs szerint ez nem kell, mert erről gondoskodik a refresh-ben.

public slots:
    void fireButtonClicked();
    void sendButtonClicked();
    void powerChanged(int value);

    /* TEMP */ void testButtonClicked();

private:
    void updateViewTransform();
    bool canMove() const;
};

#endif // GAMEWINDOW_H
