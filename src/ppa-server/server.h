#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <stdlib.h>
#include "messagetypes.h"

struct Player;

struct Game {
    QString name;
    Player* player1 = NULL, *player2 = NULL;
};

struct Player {
    QString name;
    Game* game = NULL;
    QTcpSocket* socket = NULL;
};

class Server : public QObject {
    Q_OBJECT

private:
    QTcpServer serverSocket;
    QMap<QTcpSocket*, Player> players;

public:
    Server(int port);
    ~Server();

public slots:
    void newConnection();
    void receivePacket();
    void handleSocketError(QAbstractSocket::SocketError error);
private:
    Player* getOpponent(Player* player) const;
    void broadcastExcept(const QTcpSocket* senderSocket, const Message& msg);
    void sendTo(const QTcpSocket* socket, const Message& msg);
    void playerDisconnected(Player* player);
};

#endif // SERVER_H
