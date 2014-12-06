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
