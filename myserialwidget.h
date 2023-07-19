#ifndef MYSERIALWIDGET_H
#define MYSERIALWIDGET_H

#include <QWidget>
#include <QSerialPort>
#include "comservice.h"
#include <QThread>

namespace Ui {
class MySerialWidget;
}

class MySerialWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MySerialWidget(QWidget *parent = nullptr);
    void btnSendData();
    void btnClearSend();
    void btnClearRec();
    void btnSwitch();
    void dataReceive(QByteArray data);

    void saveNewData(const QString& filename);

    ~MySerialWidget();

signals:
    void cutMainWindow();

public:
    QThread mThread;
private:

    Ui::MySerialWidget *ui;

    ComService * mComservice;

    QString mBufferStr;

//    ThemeSelection        * m_theme = nullptr;
};

#endif // MYSERIALWIDGET_H
