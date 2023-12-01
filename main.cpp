#include <QApplication>
#include "frmnavbarform.h"
#include <QDebug>
#include "SerialService.h"
#include "mydataprocessing.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    frmNavBarForm z;
    z.show();
    /* 释放单例对象 */
//    SerialService::DestroyKernel();
//    MyDataProcessing::DestoryKernel();
    return a.exec();
}





