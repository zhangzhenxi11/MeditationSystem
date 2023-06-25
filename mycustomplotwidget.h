
#ifndef MYCUSTOMPLOTWIDGET_H
#define MYCUSTOMPLOTWIDGET_H

#include <QWidget>
#include "qcustomplot.h"
#include <QTimer>
#include <QTime>
#include <QThread>
#include "MySerialWidget.h"
#include "mydataprocessing.h"
#include "comservice.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MyCustomPlotWidget; }
QT_END_NAMESPACE

/* 主界面主线程绘图类 */
class MyCustomPlotWidget : public QWidget
{
    Q_OBJECT
public:
    MyCustomPlotWidget(QWidget *parent = nullptr);
    ~MyCustomPlotWidget();
public:
    /* 数据更新 */
    void drawDynamic(QCustomPlot * plot1,int plot_num,QList<QString> _idxList);

    void eegPlotsInit(QCustomPlot * plot);

    double iterationData(QVector<double> &itContainer);

    void plotRescale();

    void readColumnData(const QString &filename);

    void powerSpectrumDrawing();

signals:
    void timeout();
    void ToThread(const QString & input_filename,const QString & output_filename); // 信号


public slots: //声明槽函数
    void realTimeDataSlot();

    void newDataAvailable(int value);/* 接受串口数据的槽函数1 */

    void cusDataAvailable(char value);/* 接受串口数据的槽函数2 */

    void onCutMainWindow();          /* 窗口切换的槽函数 */

    void upDatePlot(QVector<double>& data);/* 接受数据处理类的槽函数*/

    void newSerialDatas(QByteArray data); /* 接受串口数据的槽函数3 */

    void flagfunc(int status);

private slots:
    void on_btnStart_clicked();

    void on_btnPause_clicked();

    void on_btnStop_clicked();

private:
    Ui::MyCustomPlotWidget *ui;
    QCustomPlot*                m_plot;                   //曲线指针
    QCustomPlot*                m_plotSource;             //eeg原信号
    QList<QString>              m_idxList;                //当前正在显示的曲线的编号
    QList<QString>              m_lineNames;              //曲线名称
    QList<QString>              m_powerSpectrumLines;      //曲线名称
    double                      m_value;
    MySerialWidget*             m_serialPlot = nullptr;   //串口类指针
    ComService*                 m_service = nullptr;
    MyDataProcessing*           m_dataprocessing = nullptr; //数据处理类
    QThread *                   m_customerThread ;
    QTimer *                    m_timer;                    //数据处理定时器
    QTimer *                    m_timerDynamic;             //绘图定时器
    QVector<double>
        m_deltaVect,
        m_thetaVect,
        m_alphaVect,
        m_betaVect,
        m_gamaVect;                                         // 不同波形数据容器

    int m_flag = 0;                                         // 0:关闭绘制，1：打开绘制
    bool stopRecive =false;                                 // eeg原信号标志位

    int m_status;                                           // 0:串口关闭，1：串口打开
    QRandomGenerator *          m_randInt;                  //随机

};

#endif // MYCUSTOMPLOTWIDGET_H
