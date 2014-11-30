#include <QCoreApplication>
#include <QTextStream>
#include "server.h"

#define DEFAULT_PORT 4050

/**
 * @brief The standard constructor of the server instance.
 * @param port The port which the server will use.
 */
Server::Server(int port) {
    blockSize = 0;
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
 * @brief The destructor of the server instance.
 */
Server::~Server() {
    for (QMap<QTcpSocket*, Player>::Iterator it = players.begin(); it != players.end(); ++it) {
        if (it.key() != NULL) it.key()->disconnect();
    }
    serverSocket.disconnect();
}

/**
 * @brief Handle a new incoming connection.
 */
void Server::newConnection() {
    printf("INCOMING TRANSMISSION\n");
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
 * @brief Receive a new message from a client socket.
 */
void Server::receivePacket() {
    {QTextStream st(stdout); st << "INCOMING DATA\n";}
    QTcpSocket* socket = static_cast<QTcpSocket*>(sender());
    while (socket->bytesAvailable() > 0) {
        QDataStream input(socket);
        input.setVersion(QDataStream::Qt_4_0);
        if (blockSize == 0) {
            // read the block size
            if (socket->bytesAvailable() < sizeof(quint32)) return;
            input >> blockSize;
        }
        // read the message itself
        if (socket->bytesAvailable() < blockSize) return;
        Message* msg = Message::readMessage(input);
        if (msg != NULL) {
            switch (msg->type) {
                case MSGT_CONNECTION_REQUEST: {
                    StringMessage* strmsg = static_cast<StringMessage*>(msg);
                    onConnectionRequest(socket, strmsg);
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
 * @param error The type of error received, only QAbstractSocket::RemoteHostClosedError is handled
 */
void Server::handleSocketError(QAbstractSocket::SocketError error) {
    QTcpSocket* socket = static_cast<QTcpSocket*>(sender());
    if (error == QAbstractSocket::RemoteHostClosedError) {
        // one of the connections has closed
        QTextStream output(stdout);
        output << QString("Connection from %1 closed\n").arg(socket->peerAddress().toString());
        Player* player = &players[socket];
        playerDisconnected(player);
        players.remove(player->socket);
        socket->close();
    } else {
        QTextStream output(stdout);
        output << "Unknown socket error\n";
    }
}

/**
 * @brief Get the opponent of a player.
 * @param player The player queried.
 * @return The opponent of the player.
 */
Player *Server::getOpponent(Player *player) const {
    if (player == NULL) return NULL;
    Game* game = player->game;
    if (game == NULL) return NULL;
    if (game->player1 == player) return game->player2;
    else if (game->player2 == player) return game->player1;
    else return NULL;
}

/**
 * @brief Send a message to everyone except the sender.
 * @param senderSocket The client to exclude, NULL if there is noone to exclude.
 * @param msg The message to be sent.
 */
void Server::broadcastExcept(const QTcpSocket* senderSocket, const Message& msg) {
    QByteArray buf;
    QDataStream outputStream(&buf, QIODevice::WriteOnly);
    outputStream.setVersion(QDataStream::Qt_4_0);
    outputStream << quint32(0) << msg;
    outputStream.device()->seek(0);
    outputStream << (quint32)(buf.size() - sizeof(quint32));
    for (QMap<QTcpSocket*, Player>::Iterator i = players.begin(); i != players.end(); ++i) {
        QTcpSocket* sock = i.key();
        if (senderSocket == sock) continue;
        sock->write(buf);
    }
}

/**
 * @brief Send a message to a client.
 * @param socket The socket of the client.
 * @param msg The message to be sent.
 */
void Server::sendTo(QTcpSocket* socket, const Message& msg) {
    QByteArray buf;
    QDataStream outputStream(&buf, QIODevice::WriteOnly);
    outputStream.setVersion(QDataStream::Qt_4_0);
    outputStream << quint32(0) << msg;
    outputStream.device()->seek(0);
    outputStream << (quint32)(buf.size() - sizeof(quint32));
    socket->write(buf);
}

/**
 * @brief Inform the opponent of a player that they have disconnected.
 * @param player The player who disconnected.
 */
void Server::playerDisconnected(Player* player) {
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
    chatMessage.str = tr("*** %1 kilépett. ***").arg(player->name);
    broadcastExcept(player->socket, chatMessage);
}

/**
 * @brief Handle a connection request.
 * @param sender The socket of the client.
 * @param msg The message packet received.
 */
void Server::onConnectionRequest(QTcpSocket* sender, StringMessage* msg) {
    bool found = false;
    for (QMap<QTcpSocket*, Player>::Iterator i = players.begin(); i != players.end(); ++i) {
        Player* p = &(i.value());
        if (p->name == msg->str) {
            found = true;
            break;
        }
    }
    if (found) {
        SimpleMessage response;
        response.type = MSGT_CONNECTION_REFUSED;
        sendTo(sender, response);
        players.remove(sender);
        QTextStream output(stdout);
        output << QString("Player %1 refused: name already taken\n").arg(msg->str);
    } else {
        SimpleMessage response;
        Player* player = &players[sender];
        player->name = msg->str;
        response.type = MSGT_CONNECTION_ACCEPTED;
        sendTo(sender, response);
        StringMessage chatMsg;
        chatMsg.type = MSGT_CHAT_MESSAGE;
        chatMsg.str = tr("*** %1 kapcsolódott.***").arg(msg->str);
        broadcastExcept(sender, chatMsg);
        QTextStream output(stdout);
        output << QString("Player %1 connected to server\n").arg(msg->str);
    }
}

/**
 * @brief The main method of the application.
 * @param argc The number of command line parameters.
 * @param argv The array of command line parameters.
 * @return The standard return value of the Qt application.
 */
int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    Server server(DEFAULT_PORT);

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
