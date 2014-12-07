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
private:
    Ui::GameWindow *ui;
    Controller* controller;

protected:
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void closeEvent(QCloseEvent *event);

public:
    explicit GameWindow(Controller* controller, QWidget *parent = 0);
    ~GameWindow();

    //TODO The followig methods are only for testing
    void setSliderValue(int value);

public slots:
    void localFireButtonClicked();
    void remoteFireButtonClicked();
};

#endif // GAMEWINDOW_H
