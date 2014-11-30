#ifndef LOBBY_H
#define LOBBY_H

class LobbyWindow;

#include <QString>
#include <QTcpSocket>
#include "messagetypes.h"
#include "lobbywindow.h"

/**
 * @brief The main class of the game lobby.
 */
class Lobby : public QObject {
    Q_OBJECT
private:
    QTcpSocket socket;
    LobbyWindow lobbywindow;

public:
    Lobby();
    ~Lobby();
    void connectToServer(const QString& nickname, const QString& address, int port);
    void showWindow();

public slots:
    void receivePacket();
    void handleSocketError(QAbstractSocket::SocketError error);

private:
    void sendMessage(const Message& msg);
};

#endif // LOBBY_H
