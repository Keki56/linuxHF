#include <Qt>
#include <QCloseEvent>
#include <QKeyEvent>
#include "gamewindow.h"
#include "ui_gamewindow.h"
#include "controller.h"
#include <QMessageBox>
#include <QTextStream>
#include <QBitmap>
#include <QtMath>

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
    QPixmap turretImage(":/sprites/turret.png");
    mask = turretImage.createMaskFromColor(Qt::white);
    turretImage.setMask(mask);
    QPixmap rightTankImage(":/sprites/tank_right_noturret.png");
    mask = rightTankImage.createMaskFromColor(Qt::white);
    rightTankImage.setMask(mask);

    // add items to the scene
    tankLeft = new QGraphicsPixmapItem(leftTankImage);
    tankLeft->setTransformOriginPoint(leftTankImage.width() / 2.0, leftTankImage.height());
    double spriteScale = 0.1 / leftTankImage.width();
    tankLeft->setScale(spriteScale);
    scene.addItem(tankLeft);
    turretLeft = new QGraphicsPixmapItem(turretImage, tankLeft);
    turretLeft->setTransformOriginPoint(0, turretImage.height() / 2.0);
    turretLeft->setPos(QPointF(leftTankImage.width() * 0.5, leftTankImage.height() * 0.15) - turretLeft->mapToParent(turretLeft->transformOriginPoint()));

    tankRight = new QGraphicsPixmapItem(rightTankImage);
    tankRight->setTransformOriginPoint(rightTankImage.width() / 2.0, rightTankImage.height());
    tankRight->setScale(spriteScale);
    scene.addItem(tankRight);
    turretRight = new QGraphicsPixmapItem(turretImage, tankRight);
    turretLeft->setTransformOriginPoint(0, turretImage.height() / 2.0);
    turretRight->setPos(QPointF(rightTankImage.width() * 0.5, rightTankImage.height() * 0.15) - turretRight->mapToParent(turretRight->transformOriginPoint()));

    tankLeft->setPos(QPointF(0.2, 1) - tankLeft->mapToParent(tankLeft->transformOriginPoint()));
    tankRight->setPos(QPointF(0.8, 1) - tankRight->mapToParent(tankRight->transformOriginPoint()));
}

/**
 * @brief Handle a keyboard event.
 * @param event The event passed to the window.
 */
void GameWindow::keyPressEvent(QKeyEvent *event){
    QMainWindow::keyPressEvent(event);
    switch (event->key()) {
    case Qt::Key_A:
        controller->onChangeLocalPosition(Controller::RIGHTtoLEFT);
        break;
    case Qt::Key_D: {
        controller->onChangeLocalPosition(Controller::LEFTtoRIGHT);
        /*QList<QGraphicsItem*> items = ui->gameView->items();
        for (QList<QGraphicsItem*>::iterator i = items.begin(); i != items.end(); ++i) {
            (*i)->setPos((*i)->pos() + QPointF(2, 0.0));
            //return;
        }*/
        // tankLeft->setPos(QPointF(0.2, 1) - tankLeft->mapToParent(tankLeft->transformOriginPoint()));
        break;
        }
    case Qt::Key_W:
        controller->onChangeLocalAngle(Controller::COUNTERCLOCKWISE);
        break;
    case Qt::Key_S:
        controller->onChangeLocalAngle(Controller::CLOCKWISE);
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
    int ret = QMessageBox::question(this, tr("PewPew Artillery"), tr("Biztosan be akarod zárni a játék ablakot?\nAz ablak bezárásával kilépsz a játékból."));
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
    // get values from the engine
    bool localLeft = engine->getLocalLeft();
    double leftPos = localLeft ? engine->getLocalPlayerPosition() : engine->getRemotePlayerPosition();
    double rightPos = localLeft ? engine->getRemotePlayerPosition() : engine->getLocalPlayerPosition();
    double leftAngle = localLeft ? engine->getLocalPlayerAngle() : engine->getRemotePlayerAngle();
    double rightAngle = localLeft ? engine->getRemotePlayerAngle() : engine->getLocalPlayerAngle();
    double leftHP = localLeft ? engine->getLocalPlayerHP() : engine->getRemotePlayerHP();
    double rightHP = localLeft ? engine->getRemotePlayerHP(): engine->getLocalPlayerHP();
    // move the sprites
    tankLeft->setPos(QPointF(leftPos, 1) - tankLeft->transformOriginPoint());
    tankRight->setPos(QPointF(rightPos, 1) - tankRight->transformOriginPoint());
    turretLeft->setRotation(-qRadiansToDegrees(leftAngle));
    turretRight->setRotation(-qRadiansToDegrees(rightAngle));
    // enable/disable GUI elements
    ui->leftHPIndicator->display(leftHP);
    ui->rightHPIndicator->display(rightHP);
    QString leftName = localLeft ? controller->getLocalPlayerName() : controller->getRemotePlayerName();
    QString rightName = localLeft ? controller->getRemotePlayerName() : controller->getLocalPlayerName();
    ui->leftPlayerLabel->setText(leftName);
    ui->rightPlayerLabel->setText(rightName);
    ui->fireButton->setEnabled(canMove());
    ui->sendButton->setEnabled(controller->hasGameStarted());
    ui->chatInputBox->setEnabled(controller->hasGameStarted());
    if (controller->hasGameStarted()) {
        if (!localLeft ^ engine->getLocalTurn()) {
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

/**
 * @brief Player pressed fire button
 */
void GameWindow::fireButtonClicked(){
    controller->onChangeLocalPower((double)ui->powerSlider->value() / 20.0);  //Maximum 5 legyen a power
    controller->fireLocalPlayer();
    //refresh();
}

/**
 * @brief Send a chat message.
 */
void GameWindow::sendButtonClicked() {
    QString message = ui->chatInputBox->text();
    if (message.isEmpty()) return;
    ui->chatInputBox->clear();
    controller->onSendChat(message);
}

/**
 * @brief Event handler called after the power was changed.
 * @param value The new value of the power.
 */
void GameWindow::powerChanged(int value) {
    controller->onChangeLocalPower(value);
}

/* TEMP */
void GameWindow::testButtonClicked() {
    controller->testAnimation();
}

/**
 * @brief Return whether the local player can make a move.
 * @return True if the local player can move.
 */
bool GameWindow::canMove() const {
    return engine->getLocalTurn() && controller->hasGameStarted();
}

/**
 * @brief Print a new line to the chat panel.
 * @param line The line to be added.
 */
void GameWindow::printChat(const QString& line) {
    ui->chatBox->appendPlainText(line);
}

