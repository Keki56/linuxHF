#include "lobbywindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LobbyWindow w;
    w.show();

    return a.exec();
}
