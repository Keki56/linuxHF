#include "lobbywindow.h"
#include "lobby.h"
#include "ui_lobbywindow.h"
#include "connecttoserverdialog.h"
#include <QMessageBox>

LobbyWindow::LobbyWindow(Lobby* lobby, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LobbyWindow),
    lobby(lobby)
{
    ui->setupUi(this);
}

LobbyWindow::~LobbyWindow() {
    delete ui;
}

void LobbyWindow::sendButtonClicked() {

}

void LobbyWindow::connectToServerClicked() {
    ConnectToServerDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        QString nickname = dialog.getNickname();
        QString address = dialog.getAddress();
        int port = dialog.getPort();
        if (nickname.isEmpty()) {
            QMessageBox::critical(this, tr("Hiba"), tr("Nincs becenév megadva!"));
            return;
        }
        if (address.isEmpty()) {
            QMessageBox::critical(this, tr("Hiba"), tr("Nincs cím megadva!"));
            return;
        }
        lobby->connectToServer(nickname, address, port);
    }
}

void LobbyWindow::disconnectClicked() {

}
