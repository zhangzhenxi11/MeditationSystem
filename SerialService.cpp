
#include "SerialService.h"
#include <QDebug>
#include <QMessageBox>

SerialService *SerialService::m_pInstance = nullptr;//初始化静态变量

SerialService::SerialService(QObject *parent)
    : QObject{parent}
{
    mSerial = new QSerialPort;
}

/* SuGaomin 2020-08-29
*
* 没有析构函数。构造函数里面new了一个类的变量，但是没有释放。
* 重复调用该类，可能会导致内存泄漏
* 当然，还有你在别的地方new的m_Instance变量
*
*/

QStringList SerialService::GetSerialAvailable()
{
    //查找可用串口
    QStringList mPortsList;
    foreach (const QSerialPortInfo& info,QSerialPortInfo::availablePorts())
    {
        mPortsList<<info.portName();
    }
    return mPortsList;
}

QStringList SerialService::GetSeriaBitAvailable()
{
    QStringList mBitList;
    auto baudRates = QSerialPortInfo::standardBaudRates();
    foreach (auto br, baudRates)
    {
        mBitList<< QString::number(br,10);
    }
    return mBitList;

}

bool SerialService::InitSerial(const QString& portname)
{
    mSerial->setPortName(portname);
    if(!mSerial->open(QIODevice::ReadWrite))
    {
        qDebug()<<"串口打开失败！";
        return false;
    }
    qDebug()<<"串口打开成功";
    /* Note: 把串口信息写死了,ui操作没用用处 ,
    办法：1.接收ui的参数
         2.整成一个ui类，打开串口函数内，参数直接跟随ui的操作改变
    */
    mSerial->setBaudRate(QSerialPort::Baud115200);
    mSerial->setDataBits(QSerialPort::Data8);
    mSerial->setParity(QSerialPort::NoParity);
    mSerial->setStopBits(QSerialPort::OneStop);

    qDebug()<< "portName"<< mSerial->portName()<< Qt::endl;
    qDebug() <<"baudRate:" << mSerial->baudRate()<< Qt::endl;
    qDebug() <<"dataBits:" << mSerial->dataBits()<< Qt::endl;
    qDebug() <<"parity:"<<mSerial->parity()<< Qt::endl;
    qDebug() <<"stopBits:" << mSerial->stopBits()<< Qt::endl;

    //读取数据
    connect(mSerial,&QSerialPort::readyRead,this,&SerialService::ReceviceSerialData);
    emitFlag(1);
    return true;
}

bool SerialService::CloseSerial()
{
    if(mSerial->isOpen())
    {
        mSerial->clear();
        mSerial->close();
        emitFlag(0);
        return true;
    }
    else
    {
        qDebug()<<"串口关闭成功！";
        return false;
    }
}


/* SuGaomin 2020-08-29
*  m_Instance 这个成员变量是放在哪里进行初始化？？？？？？
*  是放在构造函数里面。
*  你放在这里初始化，是没有效果的。有谁会调用它进行初始化一下吗？没有
*
*/

SerialService *SerialService::GetKernel()
{
    if(m_pInstance == nullptr)
    {
        m_pInstance = new SerialService();
    }
    return m_pInstance;
}

void SerialService::DestroyKernel()
{
    if(m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = nullptr;
    }
}

SerialService::~SerialService()
{
    delete mSerial;
}

/* SuGaomin 2020-08-29
 *
 * 串口接收数据，都是按照字节来接收。
 * 不需要判断大小端。
 * 其次，在代码逻辑中，你判断是小端就可以接收数据，判断大端就不接收数据了？？？
 *  一个函数仅完成一件功能原则，不要设计多用途面面俱到函数。
*/
void SerialService::ReceviceSerialData()
{
    mBuffer = mSerial->readAll();
    qInfo() << "I'm wroking in SerialService thread:" << QThread::currentThreadId();
    if(!mBuffer.isEmpty())
    {
        //校验头字节
        if(mBuffer.at(0) == 0x41 && mBuffer.at(1) == 0x41)
        {
            if(mBuffer.at(6) == 0x69 && mBuffer.at(7) == 0x69)
            {
                mResult = (mBuffer.at(2) << 24) | (mBuffer.at(3) << 16) | mBuffer.at(4)<<8| mBuffer.at(5);
                mBuffer = QByteArray::number(mResult);
                emit emitNewIntData(mBuffer.toInt()); /* 发送数据至绘图GUI */
                emit emitNewData(mBuffer);       /* 发送数据至串口GUI */
            }
        }
    }
    else
    {
        qDebug()<< QString::fromLocal8Bit("mBuffer为空！");
        return;
    }
    QThread::msleep(500); //线程休眠100ms
}

void SerialService::SendSerialData(QByteArray data)
{
    // 接收GUI数据并发送
    mSerial->write(data);
}


