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
    switch (event->key()) {
    case Qt::Key_A:
        controller->onChangePosition(Controller::RIGHTtoLEFT);
        break;
    case Qt::Key_D:
        controller->onChangePosition(Controller::LEFTtoRIGHT);
        break;
    case Qt::Key_W:
        controller->onChangeAngle(Controller::COUNTERCLOCKWISE);
        break;
    case Qt::Key_S:
        controller->onChangeAngle(Controller::CLOCKWISE);
        break;
    default:
        QMainWindow::keyPressEvent(event);
        break;
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

//TODO The followig methods are only for testing
void GameWindow::setSliderValue(int value) {
    ui->localAngleSlider->setValue(value);
}
