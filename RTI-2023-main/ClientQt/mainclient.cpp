#include "windowclient.h"

#include <QApplication>

WindowClient *w;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    w = new WindowClient();
    w->show();
    return a.exec();
}
