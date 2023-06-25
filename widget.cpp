
#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle("串口demo程序");
    serialport = new QSerialPort;
    findSerialPort();//查找可用串口
    //连接槽函数 Read_Date解析数据
    connect(serialport,&QSerialPort::readyRead,this,&Widget::ReadSerialDate);
    ui->btnSwitch->setEnabled(true); //设置控件不可用
    timSend = new QTimer;
    timSend->setInterval(1000);
    connect(timSend,&QTimer::timeout,this,&Widget::on_btnSendData_clicked);
}
Widget * Widget::m_instance=nullptr;

Widget *Widget::GetKernal()
{
    if(m_instance==nullptr)
    {
        m_instance = new Widget;
    }
    return m_instance;
}

Widget::~Widget()
{
    delete ui;
}

/*
例如：0x41      0x41      0x61       ox62      ox63      ox64      0x69       0x69
    0100 0001 0100 0001 0110 0001  0110 0010 0110 0011 0110 0100 0110 1001  0110 1001
    union{
            int a;
            char buf[200];
         }test1,test2;

    union u{
        char c[4];
        int  n;
    };
*/

/*
 * 窗口显示串口传来的数据并转成int值
 * 16进制：两个8位共16位组成，一个8位是一个字节就是一个字符。4：0000 0100 1：0000 0001
 * 串口传输：一个一个字节传输的。
 * 此处接收地数据要转成是一个个int值，不考虑是否16进制接收。
*/
void Widget::ReadSerialDate()
{
    int ret = serialport->read(buf,200); //最多将 maxSize 字节从设备读取到数据中，并返回读取的字节数
    if(ret!= -1)
    {
        //先判断大小端
        bool islittle = isLittleEndian();//本机是小端
        if(islittle)
        {
            QString str = ui->edtReceive->toPlainText();
            //校验头字节
            if(buf[0] == 0x41 && buf[1] == 0x41)
            {
                //校验尾字节
                if(buf[6] == 0x69 && buf[7] == 0x69)
                {
                    //(0x61<<24) | (0x62<<16) | (0x63<<8) | (0x64)
                    result = (buf[2] << 24) | (buf[3] << 16) | buf[4]<<8| buf[5];
                    m_targetStr = QString::number(result,10);//62 63 64 69
                    /* 发送信号 */
                    emit prouctNewData(m_targetStr.toInt());
                    ui->edtReceive->insertPlainText(m_targetStr);
                    // 移动光标到文本结尾
                    ui->edtReceive->moveCursor(QTextCursor::End);
                }
                else
                {
                    Prompt_Information();
                }
            }
            else
            {
                Prompt_Information();
            }
        }
    }
//    memset(buf,'\0',sizeof(buf));//清空缓存区，
}

void Widget::findSerialPort()
{
    //查找可用串口
    foreach (const QSerialPortInfo& info,QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);   //设置串口
        if (serial.open(QIODevice::ReadWrite))//以读写方式打开串口
        {
            ui->cmbSerial->addItem(serial.portName()); //显示串口name
            auto baudRates = QSerialPortInfo::standardBaudRates();
            for(auto br : baudRates)
            {
                ui->cmbBit->addItem(QString::number(br),br);
            }
            const QString & str = ui->cmbBit->currentText();
            connect(ui->cmbBit,static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged),
                    this,[=](const QString &str)
                    {
                        serialport->setBaudRate(str.toInt());//设置波特率
                    });
        }
    }
}
//延时函数
void Widget::sleep(int msec)
{
    //非阻塞延时
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while(QTime::currentTime()<dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);//不停地处理事件，让程序保持响应
}

bool Widget::isLittleEndian()
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

void Widget::on_btnSendData_clicked()
{
    QByteArray sendData;
    /* 判断是否为16进制发送，
     * 将发送区全部的asc2转换为16进制字符串显示，
     * 发送的时候转换为16进制发送
     */
    if(ui->chkSend->checkState() == false)
    {
        sendData = ui->edtSend->toPlainText().toLocal8Bit(); /*  GB2312编码输出 */
        //sendData = ui->edtSend->toPlainText().toUtf8();    /* Unicode编码输出 */
        //sendData = ui->edtSend->toPlainText().toLatin1();  /* ascii编码发送 */
    }
    else
    {
        sendData =QByteArray::fromHex(ui->edtSend->toPlainText().toLocal8Bit());
    }
    serialport->write(sendData);

    /*
    //默认16进制
    QString str = ui->edtSend->toPlainText();
    ui->edtSend->clear();
    ui->edtSend->append(str);
    //给下位机发数据 用到的是QIODevice::write
    serialport->write(ui->edtSend->toPlainText().toLatin1());
    */

}

void Widget::Prompt_Information()
{
    QMessageBox::information(this,tr("Erro"),tr("检查发送信息格式"),QMessageBox::Ok);
}

void Widget::on_btnSwitch_clicked()
{
    update();
    sleep(100);
    //初始化串口
    serialport->setPortName(ui->cmbSerial->currentText());//设置串口名
    if(ui->btnSwitch->text() == "打开串口")
    {
        //获取标准的波特率
        if(serialport->open(QIODevice::ReadWrite))//用ReadWrite 的模式尝试打开串口
        {
            ui->btnSwitch->setText("关闭串口");
            /* 选择功能不可用，控件背景为灰 */
            ui->cmbBit->setEnabled(false);
            ui->cmbCheck->setEnabled(false);
            ui->cmbDataBit->setEnabled(false);
            ui->cmbSerial->setEnabled(false);
            ui->cmbStop->setEnabled(false);
//            serialport->setFlowControl(QSerialPort::NoFlowControl); //设置流控制
//            //连接槽函数 Read_Date解析数据
//            connect(serialport,&QSerialPort::readyRead,this,&Widget::ReadSerialDate);
        }
        else //打开失败
        {
            sleep(100);
            QMessageBox::information(this,tr("Erro"),tr("Open the failure"),QMessageBox::Ok);
        }
    }
    else
    {
        ui->btnSwitch->setText("打开串口");
        serialport->clear();        //清空缓存区
        serialport->close();        //关闭串口
        // 端口号下拉框恢复可选，避免误操作
        ui->cmbBit->setEnabled(true);
        ui->cmbCheck->setEnabled(true);
        ui->cmbDataBit->setEnabled(true);
        ui->cmbSerial->setEnabled(true);
        ui->cmbStop->setEnabled(true);
    }
}

void Widget::on_btnClearSend_clicked()
{
    ui->edtSend->clear();
}


void Widget::on_btnClearRec_clicked()
{
    ui->edtReceive->clear();
}

/* 复选框选中则接受的数据转换为GB2312编码16进制形式，否则unicode编码地字符串显示 */
void Widget::on_chkRec_stateChanged(int arg1)
{
    QString textbuf = ui->edtReceive->toPlainText();
    /* 获取多选框状态，未选为0,选中为1 */
    if(arg1 == 0)
    {
        /* 把gb2312编码转换成unicode */
        QString str1 = QTextCodec::codecForName("GB2312")->toUnicode(QByteArray::fromHex(textbuf.toLocal8Bit()));
        /* 文本控件清屏，显示新文本 */
        ui->edtReceive->clear();
        ui->edtReceive->insertPlainText(str1);
        /* 移动光标到文本末尾 */
        ui->edtReceive->moveCursor(QTextCursor::End);
    }
    else
    {
        /* 不为0时，多选框被勾选，接收区先前接收asc2字符串转换为16进制显示 */
        QString str1 = textbuf.toLocal8Bit().toHex().toUpper(); // GB2312编码
        // 添加空格
        QByteArray str2;
        for(int i = 0; i< str1.length (); i+=2)
        {
            str2 += str1.mid (i,2);
            str2 += " ";
        }
        /* 文本控件清屏，显示新文本 */
        ui->edtReceive->clear();
        ui->edtReceive->insertPlainText(str2);
        /* 移动光标到文本末尾 */
        ui->edtReceive->moveCursor(QTextCursor::End);
    }
}

/* 复选框选中则发送的数据转换为GB2312编码16进制形式，否则转换为unicode编码地字符串 */
void Widget::on_chkSend_stateChanged(int arg1)
{
    // 获取文本字符串
    QString txtBuf = ui->edtSend->toPlainText();
    /* 为0时，多选框未被勾选，将先前的发送区的16进制字符串转换为asc2字符串 */
    if(arg1 == 0)
    {
        /* 把gb2312编码转换成unicode */
        QTextCodec* codeText = QTextCodec::codecForName("Gb2312");
        QString str1 = codeText->toUnicode(QByteArray::fromHex(txtBuf.toLocal8Bit()));
        // 文本控件清屏，显示新文本
        ui->edtSend->clear();
        ui->edtSend->insertPlainText(str1);
        // 移动光标到文本结尾
        ui->edtSend->moveCursor(QTextCursor::End);
    }
    else
    {
        QString str1 = txtBuf.toLocal8Bit().toHex().toUpper();// GB2312编码输出
        // 添加空格
        QString str2;
        for(int i = 0; i<str1.length (); i+=2)
        {
            str2 += str1.mid (i,2);
            str2 += " ";
        }
        // 文本控件清屏，显示新文本
        ui->edtSend->clear();
        ui->edtSend->insertPlainText(str2);
        // 移动光标到文本结尾
        ui->edtSend->moveCursor(QTextCursor::End);
    }







}

/* 编辑框不得小于20ms*/
void Widget::on_chkTimSend_stateChanged(int arg1)
{
    if(arg1 == 0)
    {
        timSend->stop();
        ui->txtSendMs->setEnabled(true);
    }
    else
    {
        timSend->start(ui->txtSendMs->text().toInt());
        ui->txtSendMs->setEnabled(false);
    }

}

