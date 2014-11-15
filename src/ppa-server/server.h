#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QTextStream>
#include <QMap>
#include <stdlib.h>

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
    QTextStream output;
    QTcpServer serverSocket;
    QMap<QTcpSocket*, Player> players;

public:
    Server(int port);
    ~Server();

public slots:
    void newConnection();
    void receivePacket();

private:
    Player* getOpponent(Player* player) const;
};

#endif // SERVER_H
