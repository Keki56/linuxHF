#include <Qt>
#include <QCloseEvent>
#include <QKeyEvent>
#include "gamewindow.h"
#include "ui_gamewindow.h"
#include "controller.h"

GameWindow::GameWindow(Controller* controller, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GameWindow),
    controller(controller)
{
    ui->setupUi(this);
    connect(ui->localFireButton, SIGNAL(clicked()), SLOT(localFireButtonClicked()));
    connect(ui->remoteFireButton, SIGNAL(clicked()), SLOT(remoteFireButtonClicked()));
}

void GameWindow::keyPressEvent(QKeyEvent *event){
    if (Qt::Key_A == event->key()) {
        controller->onChangePosition(Controller.RIGHTtoLEFT);
    } else if (Qt::Key_D == event->key()) {
        controller->onChangePosition(Controller.LEFTtoRIGHT);
    } else {
        QMainWindow.keyPressEvent(event);
    }
}

void GameWindow::closeEvent(QCloseEvent *event){
    controller->onWindowClosed();
    event->accept();
}

GameWindow::~GameWindow()
{
    delete ui;
}

void GameWindow::localFireButtonClicked(){
    controller->fireLocalPlayer();
}

void GameWindow::remoteFireButtonClicked(){
    controller->fireRemotePlayer();
}
