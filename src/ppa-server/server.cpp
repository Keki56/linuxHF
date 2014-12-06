#include <QCoreApplication>
#include <QTextStream>
#include <QStringList>
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
    for (QMap<QTcpSocket*, Player>::iterator it = players.begin(); it != players.end(); ++it) {
        if (it.key() != NULL) it.key()->disconnect();
    }
    serverSocket.disconnect();
}

/**
 * @brief Handle a new incoming connection.
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
 * @brief Receive a new message from a client socket.
 */
void Server::receivePacket() {
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
                case MSGT_CHAT_MESSAGE: {
                    broadcastExcept(socket, *msg);
                    break;
                }
                case MSGT_NEW_GAME: {
                    StringMessage* strmsg = static_cast<StringMessage*>(msg);
                    newGame(&players[socket], strmsg->str);
                    break;
                }
                case MSGT_JOIN_GAME: {
                    StringMessage* strmsg = static_cast<StringMessage*>(msg);
                    if (games.contains(strmsg->str)) {
                        joinGame(&players[socket], &games[strmsg->str]);
                    }
                    break;
                }
                case MSGT_GAME_CLOSED: {
                    closeGame(&players[socket]);
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
        socket->deleteLater();
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
    for (QMap<QTcpSocket*, Player>::iterator i = players.begin(); i != players.end(); ++i) {
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
    closeGame(player);
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
 * @brief Create a new game on the server.
 * @param host The player who creates the game.
 */
void Server::newGame(Player* host, const QString& gameName) {
    QTextStream output(stdout);
    output << QString("Game started: %1 by %2\n").arg(gameName, host->name);
    if (games.contains(host->name)) return;
    Game* game = &games[host->name];
    game->name = gameName;
    game->player1 = host;
    host->game = game;
    // broadcast the start of the new game to other players
    StringMessage msg;
    msg.type = MSGT_GAME_CREATED;
    msg.str = host->name + "\n" + gameName;
    broadcastExcept(host->socket, msg);
}

/**
 * @brief End a game on the server.
 * @param game The game to be ended.
 */
void Server::endGame(Game* game) {
    QTextStream output(stdout);
    output << QString("Game by %1 ended\n").arg(game->player1->name);
    if (game->player2 != NULL) {
        game->player2->game = NULL;
    } else {
        // the game wasn't even started
        StringMessage msg;
        msg.type = MSGT_GAME_REMOVED;
        msg.str = game->player1->name;
        broadcastExcept(game->player1->socket, msg);
    }
    game->player1->game = NULL;
    games.remove(game->player1->name);
}

/**
 * @brief Make a player join an open game.
 * @param player The player who joins the game.
 * @param game The game to be joined.
 */
void Server::joinGame(Player* player, Game* game) {
    QTextStream output(stdout);
    output << QString("%1 joined the game of %2\n").arg(player->name, game->player1->name);
    if (game->player2 != NULL) return; // the game has already started
    game->player2 = player;
    player->game = game;
    // tell all players that the game should be removed from the list of open games
    StringMessage msg;
    msg.type = MSGT_GAME_REMOVED;
    msg.str = game->player1->name;
    broadcastExcept(NULL, msg);
    // tell the players that the game has started
    msg.type = MSGT_GAME_STARTED;
    sendTo(player->socket, msg);
    msg.str = player->name;
    sendTo(game->player1->socket, msg);
}

/**
 * @brief Called after a player has closed a game.
 * @param player The player who closed the game.
 */
void Server::closeGame(Player* player) {
    Game* game = player->game;
    if (game == NULL) return; // the game was already closed
    QString hostName = game->player1->name;
    if (game->player2 == NULL) {
        // it was an open game
        StringMessage msg;
        msg.type = MSGT_GAME_REMOVED;
        msg.str = hostName;
        broadcastExcept(player->socket, msg);
    } else {
        // the game has already started
        Player* opp = getOpponent(player);
        SimpleMessage msg;
        msg.type = MSGT_GAME_CLOSED;
        sendTo(opp->socket, msg);
        opp->game = NULL;
    }
    player->game = NULL;
    games.remove(hostName);
}

/**
 * @brief Handle a connection request.
 * @param sender The socket of the client.
 * @param msg The message packet received.
 */
void Server::onConnectionRequest(QTcpSocket* sender, StringMessage* msg) {
    bool found = false;
    for (QMap<QTcpSocket*, Player>::iterator i = players.begin(); i != players.end(); ++i) {
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
        sender->deleteLater();
        QTextStream output(stdout);
        output << QString("Player %1 refused: name already taken\n").arg(msg->str);
    } else {
        SimpleMessage response;
        Player* player = &players[sender];
        player->name = msg->str;
        response.type = MSGT_CONNECTION_ACCEPTED;
        sendTo(sender, response);
        // send the list of open games
        for (QMap<QString, Game>::iterator i = games.begin(); i != games.end(); ++i) {
            Game* g = &i.value();
            if (g->player2 != NULL) continue;
            StringMessage gameMsg;
            gameMsg.type = MSGT_GAME_CREATED;
            gameMsg.str = g->player1->name + "\n" + g->name;
            sendTo(sender, gameMsg);
        }
        // send a chat message
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

    return a.exec();
}
