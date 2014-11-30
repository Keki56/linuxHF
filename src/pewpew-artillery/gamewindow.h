#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
class Controller;
//#include "controller.h"

namespace Ui {
class GameWindow;
}

class GameWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit GameWindow(Controller* controller, QWidget *parent = 0);
    ~GameWindow();

private:
    Ui::GameWindow *ui;
    Controller* controller;

public slots:
    void localFireButtonClicked(bool checked);
};

#endif // GAMEWINDOW_H
