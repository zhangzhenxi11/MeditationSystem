#include "myserialwidget.h"
#include "ui_myserialwidget.h"
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QTextCodec>

MySerialWidget::MySerialWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MySerialWidget)
{
    ui->setupUi(this);
    connect(ui->btnSwitch,&QPushButton::clicked,this,&MySerialWidget::btnSwitch);
    connect(ui->btnClearRec,&QPushButton::clicked,this,&MySerialWidget::btnClearRec);
    connect(ui->btnClearSend,&QPushButton::clicked,this,&MySerialWidget::btnClearSend);
    connect(ui->btnSendData,&QPushButton::clicked,this,&MySerialWidget::btnSendData);

    qInfo() << "MySerialWidget's thread is :" << QThread::currentThreadId();
    // 获得可用串口类指针
    mComservice = SerialService::GetKernel();

    // 填写串口信息
    fillPortsInfo();

    //1,把工作类移动到线程
    mComservice->moveToThread(&mThread);
    //2,连接信号
    connect(this,&MySerialWidget::startThread,mComservice,&SerialService::ReceviceSerialData);
    //3,开启线程(线程处理函数，并没有执行)
    mThread.start(QThread::HighPriority);
    //4,主线程发送开始信号
    startThreadRunning();

}

void MySerialWidget::btnSendData()
{
    QByteArray dataSend;
    QString str = ui->edtSend->toPlainText()+ "\r\n";
    dataSend = str.toUtf8();
    mComservice->SendSerialData(dataSend);
}

void MySerialWidget::btnClearSend()
{
    ui->edtSend->clear();
}

void MySerialWidget::btnClearRec()
{
    ui->edtReceive->clear();
}

/* 设置串口基本信息，打开成功：  失败： */
void MySerialWidget::btnSwitch()
{
    if(ui->btnSwitch->text() == "打开串口")
    {
        if(!mComservice->InitSerial(ui->cmbSerial->currentText()))
        {
            QMessageBox::information(this,"打开串口","串口打开失败");
            return;
            /* SuGaomin 2020-08-29
             *  串口打开失败，就直接return 掉了？？？
             *  串口配置的一些信息不clear掉？？？？？
             *  串口配置的信息不清除掉，导致下次再打开串口的时候，还是会导致串口出错？
             *   mComservice->CloseSerial();
             */
        }
        else
        {
            /* 重新设置串口基本信息 */
            ui->btnSwitch->setText("关闭串口");
            QMessageBox::information(this,"打开串口","串口打开成功");
        }
        /* 获取串口数据并处理：保存到本地和ui显示 */
        connect(mComservice,&SerialService::emitNewData,this,&MySerialWidget::dataReceive,Qt::ConnectionType::DirectConnection);
    }
    else
    {
       ui->btnSwitch->setText("打开串口");
       mComservice->CloseSerial();
    }
}

void MySerialWidget::dataReceive(QByteArray data)
{
    qInfo() << __FUNCTION__ << QThread::currentThreadId();
    mBufferStr = QString(data)+ "\n"; //uncoide 编码
    saveNewData("../FFTW/relax/rawData.txt");

    ui->edtReceive->insertPlainText(mBufferStr);
    ui->edtReceive->moveCursor(QTextCursor::End);
}

void MySerialWidget::saveNewData(const QString & filename)
{
    QFile file(filename);
    bool ret = file.open(QIODevice::Append | QIODevice::Text);//字符串
    if(ret)
    {
       QTextStream out(&file);
       out.setCodec("UTF-8");
       out<< mBufferStr;
       file.close();
    }
}

void MySerialWidget::startThreadRunning()
{
    emit startThread();
}


MySerialWidget::~MySerialWidget()
{
    mThread.wait();
    mThread.quit();
    delete ui;
}

void MySerialWidget::fillPortsInfo()
{
    //设置串口
    QStringList mslist = mComservice->GetSerialAvailable();
    ui->cmbSerial->addItems(mslist);

    //设置波特率
    QStringList  mBitlist = mComservice->GetSeriaBitAvailable();
    ui->cmbBit->addItems(mBitlist);
    ui->cmbBit->setCurrentText("152000");

    //设置数据位
    ui->cmbDataBit->addItem("5",QSerialPort::Data5);
    ui->cmbDataBit->addItem("6",QSerialPort::Data6);
    ui->cmbDataBit->addItem("7",QSerialPort::Data7);
    ui->cmbDataBit->addItem("8",QSerialPort::Data8);
    ui->cmbDataBit->setCurrentText("8");


    //设置校验位
    ui->cmbCheck->addItem("NoParity",QSerialPort::NoParity);
    ui->cmbCheck->addItem("EvenParity",QSerialPort::EvenParity);
    ui->cmbCheck->addItem("OddParity",QSerialPort::OddParity);
    ui->cmbCheck->addItem("SpaceParity",QSerialPort::SpaceParity);
    ui->cmbCheck->addItem("MarkParity",QSerialPort::MarkParity);

    //设置停止位
    ui->cmbStop->addItem("1",QSerialPort::OneStop);
    ui->cmbStop->addItem("1.5",QSerialPort::OneAndHalfStop);
    ui->cmbStop->addItem("2",QSerialPort::TwoStop);
}

void MySerialWidget::baudRateChanged()
{

}





