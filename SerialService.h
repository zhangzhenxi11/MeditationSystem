
#ifndef SERIALSERVICE_H
#define SERIALSERVICE_H

/* 串口服务类创建 */
#include <QDebug>
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>


/* SuGaomin 2020-08-29
   *
   * 这里为什么要用 SerialService??? 为什么不改成SerialService？？？
   * 因为你里面的成员函数，都是用的Serial代表串口。确保同一个东西，英文翻译一致。
  */

class SerialService : public QObject
{
    Q_OBJECT

public:
    explicit SerialService(QObject *parent = nullptr);

    QStringList GetSerialAvailable();

    QStringList GetSeriaBitAvailable();

    bool InitSerial(const QString& portname);

    bool CloseSerial();

    static SerialService* GetKernel(); /* 获取单实例对象 */

    static void DestroyKernel();

    ~SerialService();

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
    static SerialService *m_pInstance;
    int mResult = 0;
    QString  mTargetStr; //转换后的值

};

#endif // SERIALSERVICE_H
