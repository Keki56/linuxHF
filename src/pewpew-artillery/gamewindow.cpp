#include <Qt>
#include <QCloseEvent>
#include <QKeyEvent>
#include "gamewindow.h"
#include "ui_gamewindow.h"
#include "controller.h"
#include <QMessageBox>
#include <QTextStream>
#include <QBitmap>

/**
 * @brief The main constructor of the main window.
 * @param controller The Controller instance.
 * @param engine The GameEngine instance.
 * @param parent The QWidget parent of this window.
 */
GameWindow::GameWindow(Controller* controller, GameEngine* engine, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GameWindow),
    controller(controller),
    engine(engine)
{
    ui->setupUi(this);
    scene.setSceneRect(0, 0, 1, 1);
    ui->gameView->setScene(&scene);

    // load images
    QPixmap leftTankImage(":/sprites/tank_left_noturret.png");
    QBitmap mask = leftTankImage.createMaskFromColor(Qt::white);
    leftTankImage.setMask(mask);
    QPixmap leftTurretImage(":/sprites/turret_left.png");
    mask = leftTurretImage.createMaskFromColor(Qt::white);
    leftTurretImage.setMask(mask);
    QPixmap rightTankImage(":/sprites/tank_right_noturret.png");
    mask = rightTankImage.createMaskFromColor(Qt::white);
    rightTankImage.setMask(mask);
    QPixmap rightTurretImage(":/sprites/turret_right.png");
    mask = rightTankImage.createMaskFromColor(Qt::white);
    rightTurretImage.setMask(mask);

    // add items to the scene
    tankLeft = new QGraphicsPixmapItem(leftTankImage);
    tankLeft->setTransformOriginPoint(leftTankImage.width() / 2.0, leftTankImage.height());
    double spriteScale = 0.1 / leftTankImage.width();
    tankLeft->setScale(spriteScale);
    scene.addItem(tankLeft);
    turretLeft = new QGraphicsPixmapItem(leftTurretImage, tankLeft);
    turretLeft->setTransformOriginPoint(0, leftTurretImage.height() / 2.0);
    turretLeft->setPos(QPointF(leftTankImage.width() * 0.5, leftTankImage.height() * 0.15) - turretLeft->mapToParent(turretLeft->transformOriginPoint()));

    tankRight = new QGraphicsPixmapItem(rightTankImage);
    tankRight->setTransformOriginPoint(rightTankImage.width() / 2.0, rightTankImage.height());
    tankRight->setScale(spriteScale);
    scene.addItem(tankRight);
    turretRight = new QGraphicsPixmapItem(rightTurretImage, tankRight);
    turretRight->setTransformOriginPoint(rightTurretImage.width(), rightTurretImage.height() / 2.0);
    turretRight->setPos(QPointF(rightTankImage.width() * 0.5, rightTankImage.height() * 0.15) - turretRight->mapToParent(turretRight->transformOriginPoint()));
}

/**
 * @brief Handle a keyboard event.
 * @param event The event passed to the window.
 */
void GameWindow::keyPressEvent(QKeyEvent *event){
    QTextStream output(stdout);
    switch (event->key()) {
    case Qt::Key_A:
        output << "A";
        controller->onChangePosition(Controller::RIGHTtoLEFT);
        break;
    case Qt::Key_D:
        output << "D";
        controller->onChangePosition(Controller::LEFTtoRIGHT);
        break;
    case Qt::Key_W:
        output << "W";
        controller->onChangeAngle(Controller::COUNTERCLOCKWISE);
        break;
    case Qt::Key_S:
        output << "S";
        controller->onChangeAngle(Controller::CLOCKWISE);
        break;
    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}

/**
 * @brief Handle a window close event.
 * @param event The event passed to the window.
 */
void GameWindow::closeEvent(QCloseEvent *event){
    int ret = QMessageBox::warning(this, tr("PewPew Artillery"), tr("Biztosan be akarja zárni a játék ablakot?\nAz ablak bezárásával kilép a játékból."),
                                  QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
    if (ret == QMessageBox::Yes) {
        event->accept();
        controller->onWindowClosed();
    } else {
        event->ignore();
    }
}

/**
 * @brief Handle a window resize event.
 * @param event The event received from the system.
 */
void GameWindow::resizeEvent(QResizeEvent* event) {
    updateViewTransform();
    QMainWindow::resizeEvent(event);
}

/**
 * @brief Event handler called after the window was shown.
 * @param event The event received from the system.
 */
void GameWindow::showEvent(QShowEvent* event) {
    updateViewTransform();
    QMainWindow::showEvent(event);
}

/**
 * @brief Update the game view according to its current size.
 */
void GameWindow::updateViewTransform(){
    ui->gameView->resetTransform();
    ui->gameView->scale(ui->gameView->geometry().width(), ui->gameView->geometry().height());
    ui->gameView->centerOn(0.5, 0.5);
}

/**
 * @brief The destructor of the game window.
 */
GameWindow::~GameWindow() {
    delete ui;
}

/**
 * @brief Refresh the game window according to the game engine.
 */
void GameWindow::refresh() {
    bool localLeft = engine->getLocalLeft();
    double leftPos = localLeft ? engine->getLocalPlayerPosition() : engine->getRemotePlayerPosition();
    double rightPos = localLeft ? engine->getRemotePlayerPosition() : engine->getLocalPlayerPosition();
    double leftAngle = localLeft ? engine->getLocalPlayerAngle() : engine->getRemotePlayerAngle();
    double rightAngle = localLeft ? engine->getRemotePlayerAngle() : engine->getLocalPlayerAngle();
    double leftHP = localLeft ? engine->getLocalPlayerHP() : engine->getRemotePlayerHP();
    double rightHP = localLeft ? engine->getRemotePlayerHP(): engine->getLocalPlayerHP();
    // tankLeft->setPos(QPointF(leftPos, 1) - tankLeft->mapToParent(tankLeft->transformOriginPoint()));
    // tankRight->setPos(QPointF(rightPos, 1) - tankRight->mapToParent(tankRight->transformOriginPoint()));
    tankLeft->setPos(QPointF(0.2, 1) - tankLeft->mapToParent(tankLeft->transformOriginPoint()));
    tankRight->setPos(QPointF(0.8, 1) - tankRight->mapToParent(tankRight->transformOriginPoint()));
    // TODO: set right position, angles
    ui->leftHPIndicator->display(leftHP);
    ui->rightHPIndicator->display(rightHP);
    ui->fireButton->setEnabled(canMove());
    ui->sendButton->setEnabled(controller->hasGameStarted());
    ui->chatInputBox->setEnabled(controller->hasGameStarted());
    if (controller->hasGameStarted()) {
        if (localLeft && engine->getLocalTurn()) {
            ui->leftPlayerLabel->setFrameStyle(QFrame::Box);
            ui->rightPlayerLabel->setFrameStyle(QFrame::NoFrame);
        } else {
            ui->leftPlayerLabel->setFrameStyle(QFrame::NoFrame);
            ui->rightPlayerLabel->setFrameStyle(QFrame::Box);
        }
    } else {
        ui->leftPlayerLabel->setFrameStyle(QFrame::NoFrame);
        ui->rightPlayerLabel->setFrameStyle(QFrame::NoFrame);
    }
}

/**
 * @brief Set the fire ui elements enabled
 */
void GameWindow::setFireEnabled(bool enabled){
    ui->gameControlsPanel->setEnabled(enabled);
}

void GameWindow::fireButtonClicked(){
    controller->fireLocalPlayer();
}

/**
 * @brief Send a chat message.
 */
void GameWindow::sendButtonClicked() {

}

/**
 * @brief Return whether the local player can make a move.
 * @return True if the local player can move.
 */
bool GameWindow::canMove() const {
    return engine->getLocalTurn() && controller->hasGameStarted();
}
