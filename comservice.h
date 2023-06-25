
#ifndef COMSERVICE_H
#define COMSERVICE_H

/* 串口服务类创建 */
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>

class ComService : public QObject
{
    Q_OBJECT
public:
    explicit ComService(QObject *parent = nullptr);

    QStringList GetSerialAvailable();

    QStringList GetSeriaBitAvailable();

    bool InitSerial(QString portname);

    bool CloseSerial();

    bool isLittleEndian();               //判断是否小端存储

    static ComService* GetKernel();

signals:
    void emitNewData(QByteArray data);
    void emitNewIntData(int data);
    void emitFlag(int status);

public slots:

    void ReceviceSerialData();

    void SendSerialData(QByteArray data);


private:
    QSerialPort* mSerial;
    QByteArray  mBuffer;
    static ComService* m_Instance;
    int mResult = 0;
    QString  mTargetStr; //转换后的值


};

#endif // COMSERVICE_H
