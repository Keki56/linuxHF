#include "lobbywindow.h"
#include "controller.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LobbyWindow w;
    w.show();

    Controller controller(true);

    return a.exec();
}
