#include "gamewindow.h"
#include "ui_gamewindow.h"
#include "controller.h"

GameWindow::GameWindow(Controller* controller, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GameWindow),
    controller(controller)
{
    ui->setupUi(this);
    connect(ui->localFireButton, SIGNAL(clicked(bool)), SLOT(localFireButtonClicked(bool)));
}

GameWindow::~GameWindow()
{
    delete ui;
}

void GameWindow::localFireButtonClicked(bool checked){
    controller->fireLocalPlayer();
}
