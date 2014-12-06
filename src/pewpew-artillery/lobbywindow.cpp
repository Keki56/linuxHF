#include "lobbywindow.h"
#include "lobby.h"
#include "ui_lobbywindow.h"
#include "connecttoserverdialog.h"
#include <QMessageBox>
#include <QInputDialog>

/**
 * @brief The main constructor of the lobby window.
 * @param lobby A pointer to the Lobby instance.
 * @param parent The parent of this instance in the QObject hierarchy.
 */
LobbyWindow::LobbyWindow(Lobby* lobby, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LobbyWindow),
    lobby(lobby)
{
    ui->setupUi(this);
}

/**
 * @brief The destructor of the lobby window.
 */
LobbyWindow::~LobbyWindow() {
    delete ui;
}

/**
 * @brief Print a new line to the chat panel.
 * @param line The line to be added.
 */
void LobbyWindow::printChat(const QString& line) {
    ui->chatBox->appendPlainText(line);
}

/**
 * @brief Update the list of open games in the lobby window.
 * @param games The list of open games on the server.
 */
void LobbyWindow::updateGameList(const QMap<QString, QString>& games) {
    ui->gamesTable->clearContents();
    int row = 0;
    for (QMap<QString, QString>::const_iterator i = games.begin(); i != games.end(); ++i, ++row) {
        ui->gamesTable->setRowCount(row + 1);
        ui->gamesTable->setItem(row, 0, new QTableWidgetItem(i.key()));
        ui->gamesTable->setItem(row, 1, new QTableWidgetItem(i.value()));
    }
}

/**
 * @brief Add a new game to the list of open games.
 * @param playerName The name of the player who opened the game.
 * @param gameName The name of the game.
 */
void LobbyWindow::addGameToList(const QString& playerName, const QString& gameName) {
    int row = ui->gamesTable->rowCount();
    ui->gamesTable->setRowCount(row + 1);
    ui->gamesTable->setItem(row, 0, new QTableWidgetItem(playerName));
    ui->gamesTable->setItem(row, 1, new QTableWidgetItem(gameName));
}

/**
 * @brief Remove a game from the list of open games.
 * @param playerName The name of the player who started the game.
 */
void LobbyWindow::removeGameFromList(const QString& playerName) {
    int rows = ui->gamesTable->rowCount();
    for (int i = 0; i < rows; i++) {
        QTableWidgetItem* item = ui->gamesTable->item(i, 0);
        if (item->text() == playerName) {
            ui->gamesTable->removeRow(i);
            break;
        }
    }
}

/**
 * @brief Handle the closure of the window.
 * @param event The close event received from the system.
 */
void LobbyWindow::closeEvent(QCloseEvent* event) {
    if (lobby->isGameRunning()) {
        if (QMessageBox::question(this, tr("PewPew Artillery"), tr("Van futó játék folyamatban. Tényleg ki akarsz lépni?")) == QMessageBox::Yes) {
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
}

/**
 * @brief The event handler which is called when sending a chat message.
 */
void LobbyWindow::sendButtonClicked() {
    QString message = ui->chatInputBox->text();
    if (message.isEmpty()) return;
    ui->chatInputBox->clear();
    lobby->sendChat(message);
}

/**
 * @brief The event handler which is called when clicking "Connect..." in the menu.
 */
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
        nickname = nickname.replace('\n', ' ');
        lobby->connectToServer(nickname, address, port);
    }
}

/**
 * @brief The event handler which is called when clicking "Disconnect" in the menu.
 */
void LobbyWindow::disconnectClicked() {
    lobby->disconnectFromServer();
}

/**
 * @brief The event handler which is called after a successful connection to the server.
 * @param address The address of the server.
 */
void LobbyWindow::connected(const QString& address) {
    ui->newGameButton->setEnabled(true);
    ui->chatButton->setEnabled(true);
    statusBar()->showMessage(tr("Csatlakozatva a szerverhez: %1").arg(address), 5000);
}

/**
 * @brief The event handler which is called after the client has disconnected from the server.
 */
void LobbyWindow::disconnected() {
    ui->newGameButton->setEnabled(false);
    ui->joinButton->setEnabled(false);
    ui->chatButton->setEnabled(false);
    ui->chatInputBox->clear();
    ui->chatBox->clear();
    ui->gamesTable->clear();
    statusBar()->showMessage(tr("A lekapcsolódás sikeres."), 5000);
}

/**
 * @brief The event handler which is called after the player has clicked the "New game" button.
 */
void LobbyWindow::newGameClicked() {
    QString gameName = QInputDialog::getText(this, tr("Új játék"), tr("Az új játék neve:"),
                                             QLineEdit::Normal, tr("%1 játéka").arg(lobby->getPlayerName()));
    if (gameName.isNull()) return; // cancel was clicked
    if (gameName.isEmpty()) QMessageBox::critical(this, tr("Hiba"), tr("Nincs játéknév megadva!"));
    gameName = gameName.replace('\n', ' ');
    lobby->newGame(gameName);
}

/**
 * @brief The event handler which is called after the player has clicked the "Join game" button.
 */
void LobbyWindow::joinGameClicked() {
    QList<QTableWidgetItem*> selected = ui->gamesTable->selectedItems();
    for (QList<QTableWidgetItem*>::iterator i = selected.begin(); i != selected.end(); ++i) {
        if ((*i)->column() > 0) continue;
        QString hostName = (*i)->text();
        lobby->joinGame(hostName);
        return;
    }
}

/**
 * @brief The event handler which is called after a game was selected from the list of open games.
 */
void LobbyWindow::gameSelectionChanged() {
    QList<QTableWidgetItem*> selected = ui->gamesTable->selectedItems();
    ui->joinButton->setEnabled(!selected.empty());
}
