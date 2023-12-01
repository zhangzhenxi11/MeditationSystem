#ifndef MYSERIALWIDGET_H
#define MYSERIALWIDGET_H

#include <QWidget>
#include <QSerialPort>
#include "SerialService.h"
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
    void startThreadRunning();
    ~MySerialWidget();

signals:
    void startThread();

private:
    void fillPortsInfo();

private slots:
    void baudRateChanged();

public:
    QThread mThread;

private:
    Ui::MySerialWidget *ui;
    SerialService * mComservice = nullptr;
    QString mBufferStr;
    qint32 m_baudRate;

};

#endif // MYSERIALWIDGET_H
