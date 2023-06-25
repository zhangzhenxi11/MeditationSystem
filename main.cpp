#include <QApplication>
#include "frmnavbarform.h"
#include <QDir>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    frmNavBarForm z;
    z.show();
    return a.exec();
}
