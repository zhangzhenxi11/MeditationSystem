#include <QApplication>
#include "frmnavbarform.h"
#include <QDir>
#include <QDebug>
#include <QStringList>
#include <QCollator>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QStringList myStringList = (QStringList() << "abc123" << "ABc99" << "1234abcd");
    QCollator sorter;
    sorter.setNumericMode(true); // 启用数字排序模式
    sorter.setCaseSensitivity(Qt::CaseInsensitive); // 设置区分大小写模式
    std::sort(myStringList.begin(),
              myStringList.end(),
              [&](const QString& a, const QString& b)
              {
                  return sorter.compare( a, b ) < 0;
              });
    qDebug() << myStringList; // ("1234abcd", "ABc99", "abc123")

    QStringList list;
    list << "AlPha" << "beTA" << "gamma" << "DELTA";

    QMap<QString, QString> map;
    foreach (const QString &str, list)
        map.insert(str.toLower(), str);

    list = map.values();

    frmNavBarForm z;
    z.show();
    return a.exec();
}





