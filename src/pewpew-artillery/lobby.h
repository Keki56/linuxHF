#ifndef LOBBY_H
#define LOBBY_H

class LobbyWindow;

#include <QString>
#include <QMap>
#include <QTcpSocket>
#include "messagetypes.h"
#include "lobbywindow.h"
#include "controller.h"

/**
 * @brief The main class of the game lobby.
 */
class Lobby : public QObject {
    Q_OBJECT
private:
    QTcpSocket socket;
    LobbyWindow lobbywindow;
    bool connection = false;
    int blockSize = 0;
    QString playerName;
    Controller* controller = NULL;
    QMap<QString, QString> openGames;

public:
    Lobby();
    ~Lobby();
    void connectToServer(const QString& nickname, const QString& address, int port);
    void showWindow();
    void sendChat(const QString& line);
    bool isGameRunning() const;
    QString getPlayerName() const;
    void newGame(const QString& gameName);
    void joinGame(const QString& hostName);
    void removeGame(const QString& hostName);

signals:
    void connected(const QString& address);
    void disconnected();

public slots:
    void receivePacket();
    void handleSocketError(QAbstractSocket::SocketError error);

private:
    void sendMessage(const Message& msg);
    void onConnected(const QString& address);
    void onGameCreated(const QString& hostName, const QString& gameName);
    void onGameRemoved(const QString& hostName);
};

#endif // LOBBY_H
