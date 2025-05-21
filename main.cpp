
#include "tetrixwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TetrixWindow window;
    window.show();
    return app.exec();
}
