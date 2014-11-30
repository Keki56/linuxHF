#ifndef LOBBYWINDOW_H
#define LOBBYWINDOW_H

class Lobby;

#include <QMainWindow>

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

public slots:
    void sendButtonClicked();
    void connectToServerClicked();
    void disconnectClicked();
};

#endif // LOBBYWINDOW_H
