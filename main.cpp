#include <QApplication>
#include "frmnavbarform.h"
#include <QDebug>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug()<<"Main thread: "<< QThread::currentThread();
    setbuf(stdout,NULL);
    QString s = QString();
    QString ccb = QString("aaa");
    QString & strResult = s;
    const QString & cc = QString();//正确  给常量引用赋值：不管时左值还是右值
    QString & cca = ccb; //左值
//    QString & qqq = QString();
    frmNavBarForm z;
    z.show();
    return a.exec();
}





