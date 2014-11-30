#include "lobby.h"
#include "gamewindow.h"
#include <QByteArray>
#include <QDataStream>
#include <QMessageBox>
#include <stdlib.h>
#include <QApplication>

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
}

/**
 * @brief The desctructor of the Lobby object.
 */
Lobby::~Lobby() {
    socket.disconnect();
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
 * @brief Receive a new packet from the server.
 */
void Lobby::receivePacket() {

}

/**
 * @brief Handle a socket error.
 * @param error The type of the error.
 */
void Lobby::handleSocketError(QAbstractSocket::SocketError error) {

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

    GameWindow w2;
    w2.show();


    return a.exec();
}

