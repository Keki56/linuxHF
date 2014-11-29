#include <QCoreApplication>
#include <QTextStream>
#include "server.h"

#define DEFAULT_PORT 4050

/**
 * @brief Server::Server The standard constructor of the server instance.
 * @param port The port which the server will use.
 */
Server::Server(int port) {
    connect(&serverSocket, SIGNAL(newConnection()), SLOT(newConnection()));
    if (!serverSocket.listen(QHostAddress::Any, port)) {
        perror("listen");
        exit(-1);
    } else {
        QTextStream output(stdout);
        output << QString("Server listening on port %1\n").arg(port);
    }
}

/**
 * @brief Server::~Server The destructor of the server instance.
 */
Server::~Server() {
    for (QMap<QTcpSocket*, Player>::Iterator it = players.begin(); it != players.end(); ++it) {
        if (it.key() != NULL) it.key()->disconnect();
    }
    serverSocket.disconnect();
}

/**
 * @brief Server::newConnection Handle a new incoming connection.
 */
void Server::newConnection() {
    while (serverSocket.hasPendingConnections()) {
        QTcpSocket* socket = serverSocket.nextPendingConnection();
        connect(socket, SIGNAL(readyRead()), SLOT(receivePacket()));
        connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(handleSocketError(QAbstractSocket::SocketError)));
        QTextStream output(stdout);
        output << QString("New connection accepted from %1\n").arg(socket->peerAddress().toString());
        players[socket].socket = socket;
    }
}

/**
 * @brief Server::receivePacket Receive a new message from a client socket.
 */
void Server::receivePacket() {

}

/**
 * @brief Server::handleSocketError Handle a socket error.
 * @param error The type of error received, only QAbstractSocket::RemoteHostClosedError is handled
 */
void Server::handleSocketError(QAbstractSocket::SocketError error)
{
    QTcpSocket* socket = static_cast<QTcpSocket*>(sender());
    if (error == QAbstractSocket::RemoteHostClosedError) {
        // one of the connections has closed
        QTextStream output(stdout);
        output << QString("Connection from %1 closed\n").arg(socket->peerAddress().toString());
        Player* player = &players[socket];
        playerDisconnected(player);
        players.remove(socket);
        socket->close();
    } else {
        QTextStream output(stdout);
        output << "Unknown socket error\n";
    }
}

/**
 * @brief Server::getOpponent Get the opponent of a player.
 * @param player The player queried.
 * @return The opponent of the player.
 */
Player *Server::getOpponent(Player *player) const
{
    if (player == NULL) return NULL;
    Game* game = player->game;
    if (game == NULL) return NULL;
    if (game->player1 == player) return game->player2;
    else if (game->player2 == player) return game->player1;
    else return NULL;
}

/**
 * @brief Server::broadcastExcept Send a message to everyone except the sender.
 * @param senderSocket The client to exclude, NULL if there is noone to exclude.
 * @param msg The message to be sent.
 */
void Server::broadcastExcept(const QTcpSocket* senderSocket, const Message& msg)
{
    for (QMap<QTcpSocket*, Player>::Iterator i = players.begin(); i != players.end(); ++i) {
        QTcpSocket* sock = i.key();
        if (senderSocket == sock) continue;
        QDataStream outputStream(*sock);
        outputStream << msg;
    }
}

/**
 * @brief Server::sendTo Send a message to a client.
 * @param socket The socket of the client.
 * @param msg The message to be sent.
 */
void Server::sendTo(const QTcpSocket* socket, const Message& msg)
{
    QDataStream outputStream(*socket);
    outputStream << msg;
}

/**
 * @brief Server::playerDisconnected Inform the opponent of a player that they have disconnected.
 * @param player The player who disconnected.
 */
void Server::playerDisconnected(Player* player)
{
    Player* opp = getOpponent(player);
    Game* game = player->game;
    if (game != NULL) {
        if (game->player1 == player) game->player1 = NULL;
        else game->player2 = NULL;
    }
    if (opp != NULL) {
        StringMessage msg;
        msg.type = MSGT_PLAYER_DISCONNECTED;
        msg.str = player->name;
        sendTo(opp->socket, msg);
    }
    StringMessage chatMessage;
    chatMessage.type = MSGT_CHAT_MESSAGE;
    chatMessage.str = tr("%1 lekapcsolódott").arg(player->name);
    broadcastExcept(player->socket, chatMessage);
}

/**
 * @brief main The main method of the application.
 * @param argc The number of command line parameters.
 * @param argv The array of command line parameters.
 * @return The standard return value of the Qt application.
 */
int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    Server(DEFAULT_PORT);

    StringMessage sm;
    sm.str = "hello";
    QByteArray b;
    QDataStream out(&b, QIODevice::WriteOnly);
    out << sm;
    QTextStream t(stdout);
    t << b;
    t.flush();

    return a.exec();
}
