#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <stdlib.h>
#include "messagetypes.h"

struct Player;

/**
 * @brief The representation of a game running on the server.
 */
struct Game {
    QString name;
    Player* player1 = NULL, *player2 = NULL;
};

/**
 * @brief The representation of a player connected to the server.
 */
struct Player {
    QString name = "";
    Game* game = NULL;
    QTcpSocket* socket = NULL;
};

/**
 * @brief The class of the main server instance.
 */
class Server : public QObject {
    Q_OBJECT

private:
    QTcpServer serverSocket;
    QMap<QTcpSocket*, Player> players;
    QMap<QString, Game> games;
    quint32 blockSize;

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
    void sendTo(QTcpSocket* socket, const Message& msg);
    void playerDisconnected(Player* player);
    void newGame(Player* host, const QString& gameName);
    void endGame(Game* game);
    void joinGame(Player* player, Game* game);

    void onConnectionRequest(QTcpSocket* sender, StringMessage* msg);
    void onChatMessage(QTcpSocket* sender, StringMessage* msg);
};

#endif // SERVER_H
