#include "lobbywindow.h"
#include "gamewindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LobbyWindow w;
    w.show();

    GameWindow w2;
    w2.show();


    return a.exec();
}
