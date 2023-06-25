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

    //获得可用串口
    mComservice = ComService::GetKernel();
    //串口
    QStringList mslist = mComservice->GetSerialAvailable();
    ui->cmbSerial->addItems(mslist);
    //波特率
    QStringList  mBitlist = mComservice->GetSeriaBitAvailable();
    ui->cmbBit->addItems(mBitlist);
    mComservice->moveToThread(&mThread);
    mThread.start(QThread::HighPriority);
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

void MySerialWidget::btnSwitch()
{
    if(ui->btnSwitch->text() == "打开串口")
    {
        if(!mComservice->InitSerial(ui->cmbSerial->currentText()))
        {
            QMessageBox::information(this,"打开串口","串口打开失败");
            return;
        }
        ui->btnSwitch->setText("关闭串口");
        QMessageBox::information(this,"打开串口","串口打开成功");
        // 获取串口数据
       connect(mComservice,&ComService::emitNewData,this,&MySerialWidget::dataReceive);

    }
    else
    {
       ui->btnSwitch->setText("打开串口");
       mComservice->CloseSerial();
    }
}

void MySerialWidget::dataReceive(QByteArray data)
{
    mBufferStr = QString(data)+"\n"; //uncoide 编码
    ui->edtReceive->insertPlainText(mBufferStr);
    saveNewData("../FFTW/relax/rawData.txt");
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

MySerialWidget::~MySerialWidget()
{
    mThread.wait();
    mThread.quit();
    delete ui;
}




