#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
class Controller;

namespace Ui {
class GameWindow;
}

class GameWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit GameWindow(Controller* controller, QWidget *parent = 0);
    ~GameWindow();

protected:
    virtual void closeEvent(QCloseEvent *event);
private:
    Ui::GameWindow *ui;
    Controller* controller;

public slots:
    void localFireButtonClicked();
    void remoteFireButtonClicked();
};

#endif // GAMEWINDOW_H
