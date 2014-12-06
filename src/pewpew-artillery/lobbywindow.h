#ifndef LOBBYWINDOW_H
#define LOBBYWINDOW_H

#include <QMainWindow>
#include <QMap>


class Lobby;

namespace Ui {
class LobbyWindow;
}

class LobbyWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::LobbyWindow *ui;
    Lobby* lobby;

public:
    explicit LobbyWindow(Lobby* lobby, QWidget *parent = 0);
    ~LobbyWindow();
    void printChat(const QString& line);
    void updateGameList(const QMap<QString, QString>& games);
    void addGameToList(const QString& playerName, const QString& gameName);
    void removeGameFromList(const QString& playerName);

public slots:
    void sendButtonClicked();
    void connectToServerClicked();
    void disconnectClicked();
    void connected(const QString& address);
    void disconnected();
    void newGameClicked();
    void joinGameClicked();
    void gameSelectionChanged();
};

#endif // LOBBYWINDOW_H
