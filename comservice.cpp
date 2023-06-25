
#include "comservice.h"
#include <QDebug>

ComService::ComService(QObject *parent)
    : QObject{parent}
{
    mSerial = new QSerialPort;
}

QStringList ComService::GetSerialAvailable()
{
    //查找可用串口
    QStringList mPortsList;
    foreach (const QSerialPortInfo& info,QSerialPortInfo::availablePorts())
    {
        mPortsList<<info.portName();
    }
    return mPortsList;
}

QStringList ComService::GetSeriaBitAvailable()
{
    QStringList mBitList;
    auto baudRates = QSerialPortInfo::standardBaudRates();
    foreach (auto br, baudRates)
    {
        mBitList<< QString::number(br,10);
    }
    return mBitList;

}

bool ComService::InitSerial(QString portname)
{
    mSerial->setPortName(portname);
    if(!mSerial->open(QIODevice::ReadWrite))
    {
        qDebug()<<"串口打开失败！";
        return false;
    }
    qDebug()<<"串口打开成功";
    mSerial->setBaudRate(QSerialPort::Baud115200);
    mSerial->setDataBits(QSerialPort::Data8);
    mSerial->setParity(QSerialPort::NoParity);
    mSerial->setStopBits(QSerialPort::OneStop);
    //读取数据
    connect(mSerial,&QSerialPort::readyRead,this,&ComService::ReceviceSerialData);
    emitFlag(1);
    return true;

}

bool ComService::CloseSerial()
{
    if(mSerial->isOpen())
    {
        mSerial->clear();
        mSerial->close();
        emitFlag(0);
        return true;
    }
    else
        return false;
    qDebug()<<"串口关闭成功！";
}

bool ComService::isLittleEndian()
{
    union
    {
        short a;
        char b;
    }test;
    test.a =0x0061;
    if(test.b=='a')return true;
    else return false;
}

ComService* ComService::m_Instance = nullptr;
ComService *ComService::GetKernel()
{
    if(m_Instance == nullptr)
    {

        m_Instance = new ComService;
    }
    return m_Instance;
}

void ComService::ReceviceSerialData()
{
    mBuffer =  mSerial->readAll();
    //先判断大小端
    bool islittle = isLittleEndian();//本机是小端
    if(islittle)
    {
        //校验头字节
        if(mBuffer.at(0) == 0x41 && mBuffer.at(1) == 0x41)
        {
            if(mBuffer.at(6) == 0x69 && mBuffer.at(7) == 0x69)
            {
                mResult = (mBuffer.at(2) << 24) | (mBuffer.at(3) << 16) | mBuffer.at(4)<<8| mBuffer.at(5);
                mBuffer = QByteArray::number(mResult);//10进制数
                emitNewIntData(mBuffer.toInt());
                // 发送数据至GUI
                emit emitNewData(mBuffer);
            }
        } 
        QThread::msleep(500); //线程休眠100ms
    }
}

void ComService::SendSerialData(QByteArray data)
{
    // 接收GUI数据并发送
    mSerial->write(data);
}

