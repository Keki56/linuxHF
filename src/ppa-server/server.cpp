#include <QCoreApplication>
#include "server.h"

#define DEFAULT_PORT 4050

Server::Server(int port) : output(stdout) {
    connect(&serverSocket, SIGNAL(newConnection()), SLOT(newConnection()));
    if (!serverSocket.listen(QHostAddress::Any, port)) {
        perror("listen");
        exit(-1);
    } else {
        (output << QString("Server listening on port %1\n").arg(port)).flush();
    }
}


Server::~Server() {
    for (QMap<QTcpSocket*, Player>::Iterator it = players.begin(); it != players.end(); ++it) {
        if (it.key() != NULL) it.key()->disconnect();
    }
    serverSocket.disconnect();
}


void Server::newConnection() {
    while (serverSocket.hasPendingConnections())
    {
        QTcpSocket* socket = serverSocket.nextPendingConnection();
        connect(socket, SIGNAL(readyRead()), SLOT(receivePacket()));
        (output << QString("New connection accepted from %1\n").arg(socket->peerAddress().toString())).flush();
        players[socket].socket = socket;
    }
}

void Server::receivePacket() {

}

Player *Server::getOpponent(Player *player) const
{
    if (player == NULL) return NULL;
    Game* game = player->game;
    if (game == NULL) return NULL;
    if (game->player1 == player) return game->player2;
    else if (game->player2 == player) return game->player1;
    else return NULL;
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    Server(DEFAULT_PORT);

    return a.exec();
}
