
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTime>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>
#include <QtEndian>
#include <QTextCodec>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

#define NUMBER_ 256   //buffer缓存数

class Widget : public QWidget
{
    Q_OBJECT

public:
    friend class UserAction;          //友元类
    Widget(QWidget *parent = nullptr);
    static Widget* GetKernal();       //单例
    ~Widget();

signals:
    void prouctNewData(int value);

private slots:
    void ReadSerialDate();                //读取串口数据
    void findSerialPort();               //查找可用串口
    void sleep(int msec);                //延时函数
    bool isLittleEndian();               //判断是否小端存储
    void on_btnSwitch_clicked();
    void on_btnSendData_clicked();
    void on_btnClearSend_clicked();
    void on_btnClearRec_clicked();
    void on_chkRec_stateChanged(int arg1);
    void on_chkSend_stateChanged(int arg1);
    void on_chkTimSend_stateChanged(int arg1);

public:
    void Prompt_Information();                  //提示框
private:
    Ui::Widget *ui;
    QSerialPort*           serialport;
    int                    result = 0;         //获取的结果
    char                   buf[NUMBER_]= {0};  //接受数据的buffer,定义并初始化
    static Widget*         m_instance;         //类指针
    QTimer*                timSend;            // 定时发送-定时器
    QString                m_targetStr;        //转换后的值


};

#endif // WIDGET_H
