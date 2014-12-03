#include "lobby.h"
#include "gamewindow.h"
#include <QByteArray>
#include <QDataStream>
#include <QMessageBox>
#include <stdlib.h>
#include <QApplication>
#include <QHostAddress>

#define CONNECTION_TIMEOUT 5000

/**
 * @brief The main constructor.
 */
Lobby::Lobby() :
    QObject(NULL),
    lobbywindow(this)
{
    connect(&socket, SIGNAL(readyRead()), SLOT(receivePacket()));
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(handleSocketError(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(connected(const QString&)), &lobbywindow, SLOT(connected(const QString&)));
    connect(this, SIGNAL(disconnected()), &lobbywindow, SLOT(disconnected()));
}

/**
 * @brief The desctructor of the Lobby object.
 */
Lobby::~Lobby() {
    socket.disconnect();
    this->disconnect();
    if (controller != NULL) delete controller;
}

/**
 * @brief Connect to the given server.
 * @param nickname The desired nickname.
 * @param address The address of the server.
 * @param port The port of the server.
 */
void Lobby::connectToServer(const QString& nickname, const QString& address, int port) {
    socket.connectToHost(address, port);
    if (socket.waitForConnected(CONNECTION_TIMEOUT)) {
        // Connected successfully
        playerName = nickname;
        StringMessage msg;
        msg.type = MSGT_CONNECTION_REQUEST;
        msg.str = nickname;
        sendMessage(msg);
    } else {
        // Could not connect
        // QMessageBox::critical(&lobbywindow, tr("Hiba"), tr("A csatlakozás nem sikerült."));
        QMessageBox::critical(&lobbywindow, tr("Hiba"), tr("%1").arg(socket.error()));
    }
}

/**
 * @brief Show the lobby window.
 */
void Lobby::showWindow() {
    lobbywindow.show();
}

/**
 * @brief Send a message through the lobby chat.
 * @param line The message to be sent.
 */
void Lobby::sendChat(const QString& line) {
    StringMessage msg;
    msg.type = MSGT_CHAT_MESSAGE;
    msg.str = QString("%1: %2").arg(playerName, line);
    lobbywindow.printChat(msg.str);
    sendMessage(msg);
}

/**
 * @brief Return whether the client is currently in a game.
 * @return True if a game is running.
 */
bool Lobby::isGameRunning() const {
    return (controller != NULL);
}

/**
 * @brief Get the name of the player.
 * @return The name of the player.
 */
QString Lobby::getPlayerName() const {
    return playerName;
}

/**
 * @brief Start a new game.
 * @param gameName The name of the game.
 */
void Lobby::newGame(const QString& gameName) {
    if (isGameRunning()) return;
    openGames[playerName] = gameName;
    lobbywindow.addGameToList(playerName, gameName);
    StringMessage msg;
    msg.type = MSGT_NEW_GAME;
    msg.str = gameName;
    sendMessage(msg);
}

/**
 * @brief Join an open game in the lobby.
 * @param hostName The name of the player who started the game.
 */
void Lobby::joinGame(const QString& hostName) {
    if (isGameRunning()) return;
    if (!openGames.contains(hostName)) return;
    StringMessage msg;
    msg.type = MSGT_JOIN_GAME;
    msg.str = hostName;
    sendMessage(msg);
}

/**
 * @brief Remove a game from the list of running games.
 * @param hostName The name of the player who started the game.
 */
void Lobby::removeGame(const QString& hostName) {
    if (!openGames.contains(hostName)) return;
    lobbywindow.removeGameFromList(hostName);
    openGames.remove(hostName);
}

/**
 * @brief Receive a new packet from the server.
 */
void Lobby::receivePacket() {
    while (socket.bytesAvailable() > 0) {
        QDataStream input(&socket);
        input.setVersion(QDataStream::Qt_4_0);
        if (blockSize == 0) {
            // read the block size
            if (socket.bytesAvailable() < sizeof(quint32)) return;
            input >> blockSize;
        }
        // read the message itself
        if (socket.bytesAvailable() < blockSize) return;
        Message* msg = Message::readMessage(input);
        if (msg != NULL) {
            switch (msg->type) {
                case MSGT_CONNECTION_ACCEPTED: {
                    onConnected(socket.peerName());
                    break;
                }
                case MSGT_CONNECTION_REFUSED: {
                    QMessageBox::critical(&lobbywindow, tr("Hiba"), tr("Csatlakozás visszautasítva"));
                    break;
                }
                case MSGT_CHAT_MESSAGE: {
                    StringMessage* strmsg = static_cast<StringMessage*>(msg);
                    lobbywindow.printChat(strmsg->str);
                    break;
                }
                case MSGT_GAME_CREATED: {
                    StringMessage* strmsg = static_cast<StringMessage*>(msg);
                    QStringList strings = strmsg->str.split('\n');
                    if (strings.length() > 0) {
                        onGameCreated(strings[0], strings[1]);
                    }
                    break;
                }
                case MSGT_GAME_REMOVED: {
                    StringMessage* strmsg = static_cast<StringMessage*>(msg);
                    onGameRemoved(strmsg->str);
                    break;
                }
                default:

                break;
            }
            delete msg;
        }
        blockSize = 0;
    }
}

/**
 * @brief Handle a socket error.
 * @param error The type of the error.
 */
void Lobby::handleSocketError(QAbstractSocket::SocketError error) {
    if (error == QAbstractSocket::RemoteHostClosedError) {
        // one of the connections has closed
        QMessageBox::critical(&lobbywindow, tr("Hiba"), tr("Megszakadt a kapcsolat a szerverrel."));
        socket.close();
    } else {
        QMessageBox::critical(&lobbywindow, tr("Hiba"), tr("Ismeretlen hálózati hiba"));
    }
}

/**
 * @brief Send a message to the server.
 * @param msg The message to be sent.
 */
void Lobby::sendMessage(const Message& msg) {
    QByteArray buf;
    QDataStream outputStream(&buf, QIODevice::WriteOnly);
    outputStream.setVersion(QDataStream::Qt_4_0);
    outputStream << quint32(0) << msg;
    outputStream.device()->seek(0);
    outputStream << (quint32)(buf.size() - sizeof(quint32));
    socket.write(buf);
}

/**
 * @brief Handle a MSGT_CONNECTION_ACCEPTED message.
 * @param address The address of the server.
 */
void Lobby::onConnected(const QString& address) {
    connection = true;
    emit connected(address);
}

/**
 * @brief Handle a MSGT_GAME_CREATED message.
 * @param hostName The name of the player who created the game.
 * @param gameName The name of the game.
 */
void Lobby::onGameCreated(const QString& hostName, const QString& gameName) {
    openGames[hostName] = gameName;
    lobbywindow.addGameToList(hostName, gameName);
}

/**
 * @brief Handle a MSGT_GAME_REMOVED message.
 * @param hostName The name of the player who created the game.
 */
void Lobby::onGameRemoved(const QString& hostName) {
    openGames.remove(hostName);
    lobbywindow.removeGameFromList(hostName);
}

/**
 * @brief The main method of the application.
 * @param argc The number of command line parameters.
 * @param argv The array of command line parameters.
 * @return The standard return value of the Qt application.
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Lobby lobby;
    lobby.showWindow();

    Controller controller(false);
    GameWindow w2(&controller);
    w2.show();

    return a.exec();
}

