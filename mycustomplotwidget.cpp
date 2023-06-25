
#include "mycustomplotwidget.h"
#include "ui_mycustomplotwidget.h"
#include <QDebug>
#include <QFile>
#include <QRandomGenerator>

MyCustomPlotWidget::MyCustomPlotWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyCustomPlotWidget)
    , m_serialPlot(new MySerialWidget)
    ,m_timer(new QTimer)
    ,m_randInt(new QRandomGenerator)
{
    ui->setupUi(this);
    //槽函数移到子线程
    m_service = ComService::GetKernel();
    m_dataprocessing  = new MyDataProcessing;
    m_customerThread = new QThread;
    QString sourcefileName = "../FFTW/relax/rawData.txt";  //选择路径
    QString outputfileName = QApplication::applicationDirPath()+ "/relax/out.txt";

    /* 数据处理类 */
    connect(this,&MyCustomPlotWidget::ToThread,m_dataprocessing,&MyDataProcessing::eegDataProcess);
    /* 数据处理类移到多线程 */
    m_dataprocessing->moveToThread(m_customerThread);
    /* m_timer 定时去发送信号,数据处理类执行算法*/
    connect(m_timer,&QTimer::timeout,this,[=]()
    {
      emit ToThread(sourcefileName,outputfileName);
    });
    /* 原始eeg信号绘图 */
    m_lineNames<<"Line1";
    /* 初始化图表 */
    eegPlotsInit(ui->customplot);
    /* 原始eeg信号连接槽函数 */
    connect(m_service,&ComService::emitNewIntData,this,&MyCustomPlotWidget::newDataAvailable);

    /* 串口发送信号，控制显示功率谱是否显示 */
    connect(m_service,&ComService::emitFlag,this,&MyCustomPlotWidget::flagfunc);

    m_timerDynamic = new QTimer;
    m_powerSpectrumLines<<"delta"<<"theta"<<"alpha"<<"beta"<<"gama";
    /* 初始化图表 */
    drawDynamic(ui->customplot_2,5,m_powerSpectrumLines);
    /* m_timerDynamic定时绘制功率谱图 */
    connect(m_timerDynamic,&QTimer::timeout,this,[=]()
   {
      powerSpectrumDrawing();
    });
    connect(&(m_serialPlot->mThread),&QThread::finished,m_service,&QObject::deleteLater);
}

MyCustomPlotWidget::~MyCustomPlotWidget()
{
    delete ui;
    m_serialPlot->mThread.wait();
    m_serialPlot->mThread.quit();
    m_customerThread->wait();
    m_customerThread->quit();
}

void MyCustomPlotWidget::powerSpectrumDrawing()
{
    if(m_flag == 1)
    {
       readColumnData("/relax/out.txt");
       realTimeDataSlot();
    }
}

void MyCustomPlotWidget::drawDynamic(QCustomPlot * plot1, int plot_num, QList<QString> _idxList)
{
    m_plot = plot1;//那个图表
    m_idxList = _idxList;//备份，可能别的函数需要它
    //设置基本坐标轴（左侧Y轴和下方X轴）可拖动、可缩放、曲线可选、legend可选、设置伸缩比例，使所有图例可见
    m_plot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom
                          |QCP::iSelectAxes|QCP::iSelectLegend|QCP::iSelectPlottables);
    //坐标轴
    for(int i = 0;i< plot_num;i++)
    {
        m_plot->addGraph();
        m_plot->graph(i)->setName(m_idxList.at(i));
        QPen pen;
        pen.setColor(QColor(std::rand()%255, std::rand()%255, std::rand()%255));
        m_plot->graph(i)->setPen(pen);       // 设置画笔
        m_plot->legend->setVisible(true); // 显示图例i
        m_plot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop|Qt::AlignLeft);//设置图例的位置
    }
    QSharedPointer<QCPAxisTickerTime>timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    m_plot->xAxis->setTicker(timeTicker);
    m_plot->axisRect()->setupFullAxesBox();
    // make left and bottom axes transfer their ranges to right and top axes:
    connect(m_plot->xAxis,SIGNAL(rangeChanged(QCPRange)),
            m_plot->xAxis2,SLOT(setRange(QCPRange)));

    connect(m_plot->yAxis,SIGNAL(rangeChanged(QCPRange)),
            m_plot->yAxis2,SLOT(setRange(QCPRange)));

    //设置选框,先开启选框
    m_plot->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);//模式：框选放大
    m_plot->selectionRect()->setPen(QPen(Qt::black,1,Qt::DashLine));//设置选框的样式：虚线
    m_plot->selectionRect()->setBrush(QBrush(QColor(0,0,100,50)));//设置选框的样式：半透明浅蓝
    m_plot->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);
    // 框选放大和拖动无法同时使用
    m_plot->setInteraction(QCP::iRangeDrag,true);//使能拖动
}

void MyCustomPlotWidget::eegPlotsInit(QCustomPlot * plot)
{
    m_plotSource = plot;
    m_plotSource->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom
                                      |QCP::iSelectAxes|QCP::iSelectLegend|QCP::iSelectPlottables);
    m_plotSource->addGraph();
    m_plotSource->graph(0)->setPen(QPen(QColor(250, 120, 0))); // 设置画笔
    m_plotSource->legend->setVisible(true);    // 显示图例
    m_plotSource->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop|Qt::AlignLeft);//设置图例的位置
    QSharedPointer<QCPAxisTickerTime>timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    m_plotSource->xAxis->setTicker(timeTicker);
    m_plotSource->axisRect()->setupFullAxesBox();
    //设置选框,先开启选框
    m_plotSource->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);//模式：框选放大
    m_plotSource->selectionRect()->setPen(QPen(Qt::black,1,Qt::DashLine));//设置选框的样式：虚线
    m_plotSource->selectionRect()->setBrush(QBrush(QColor(0,0,100,50)));//设置选框的样式：半透明浅蓝
    m_plotSource->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);
    // 框选放大和拖动无法同时使用
    m_plotSource->setInteraction(QCP::iRangeDrag,true);//使能拖动
}

double MyCustomPlotWidget::iterationData(QVector<double> & itContainer)
{
    static double first = 0;//初始量
    for( auto itor = itContainer.begin(); itor != itContainer.end(); itor++ )
    {
        if(*itor!=first)
        {
            itor= itContainer.erase(itor);
            first = *itor;
            break;
        }
    }
    return first;
}

void MyCustomPlotWidget::plotRescale()
{
    //使坐标轴可以伸缩
    QList<QCPAxis*> axes;
    axes << m_plot->yAxis2 << m_plot->xAxis2 << m_plot->yAxis << m_plot->xAxis;
    m_plot->axisRect()->setRangeZoomAxes(axes);
    //图像最高点最低点自动缩放坐标轴 y轴
    for (int i= 0; i< m_lineNames.size();++i)
    {
        m_plot->graph(i)->rescaleValueAxis();
    }
    m_plot->yAxis->rescale(true);//y轴值自动变化
//    m_plot->xAxis->setRange(key_x,8,Qt::AlignRight);
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    m_plot->replot();
}

void MyCustomPlotWidget::readColumnData(const QString &filename)
{
    using namespace std;
    {
        char filepath[1024] = { 0 };
        GetModuleFileNameA(NULL, filepath, MAX_PATH);		/* 获取到完整路径 */
        *strrchr(filepath, '\\') = '\0';					/* 截取路径 */
        ifstream ifs;                                       /* 创建流对象 */
        string path = string(filepath)+ filename.toStdString();
        ifs.open(path, ios::in);	/* 打开文件 */
        if (!ifs.is_open())						/* 判断文件是否打开 */
        {
            cout << "打开文件失败！！！";
        }
        /* 读取正确匹配特征点 */
        vector<string> item;				/* 用于存放文件中的一行数据 */
        string temp;						/* 把文件中的一行数据作为字符串放入容器中 */
        while (getline(ifs, temp))          /* 利用getline（）读取每一行，并放入到 item 中 */
        {
            item.push_back(temp);
        }
        for (auto it = item.begin(); it != item.end(); it++)
        {
            istringstream istr(*it);                 /* 其作用是把字符串分解为单词(在此处就是把一行数据分为单个数据) */
            string str;
            int count = 0;							 /* 统计一行数据中单个数据个数 */
            //获取文件中的第 0、1、2、3、4 列数据
            while (istr >> str)                      /* 以空格为界，把istringstream中数据取出放入到依次str中 */
            {
                if (count == 0)
                {
                    double data_r = stod(str);
                    m_deltaVect.push_back(data_r);
                }
                //获取第1列数据
                else if (count == 1)
                {
                    double data_r = stod(str);      /* 数据类型转换，将string类型转换成float,如果字符串不是有数字组成，则字符被转化为 0 */
                    m_thetaVect.push_back(data_r);
                }
                //获取第2列数据
                else if (count == 2)
                {
                    double data_r = stod(str);       /* 数据类型转换，将string类型转换成float */

                    m_alphaVect.push_back(data_r);
                }
                else if(count == 3)
                {
                    double data_r =  stod(str);
                    m_betaVect.push_back(data_r);
                }
                else if(count == 4)
                {
                    double data_r = stod(str);
                    m_gamaVect.push_back(data_r);
                }
                count++;
            }
        }
    }
}



void MyCustomPlotWidget::realTimeDataSlot()
{
    static QTime timeStart = QTime::currentTime();
    double key_x = timeStart.msecsTo(QTime::currentTime())/1000.0;
    static double lastPointKey = 0;
    double thetaData = iterationData(m_thetaVect);
    double deltaData = iterationData(m_deltaVect);
    double alphaData = iterationData(m_alphaVect);
    double betaData = iterationData(m_betaVect);
    double gamaData = iterationData(m_gamaVect);

    if (key_x-lastPointKey > 0.2)
    {
        m_plot->graph(0)->addData(key_x, thetaData);
        m_plot->graph(1)->addData(key_x, deltaData);
        m_plot->graph(2)->addData(key_x, alphaData);
        m_plot->graph(3)->addData(key_x, betaData);
        m_plot->graph(4)->addData(key_x, gamaData);
        lastPointKey = key_x;
    }
    m_plot->xAxis->setRange(key_x,8,Qt::AlignRight);
    plotRescale();
}

void MyCustomPlotWidget::newDataAvailable(int value)
{
    if(stopRecive)
    {
        m_value = value;
        static QTime timeStart = QTime::currentTime();
        double key_x = timeStart.msecsTo(QTime::currentTime())/1000.0;
        static double lastPointKey = 0;
        if (key_x-lastPointKey > 0.2)
        {

            m_plotSource->graph(0)->addData(key_x, m_value);
            lastPointKey = key_x;
        }
        QList<QCPAxis*> axes;
        axes << m_plotSource->yAxis2 << m_plotSource->xAxis2 << m_plotSource->yAxis << m_plotSource->xAxis;
        m_plotSource->axisRect()->setRangeZoomAxes(axes);
        //图像最高点最低点自动缩放坐标轴 y轴
        for (int i= 0; i< m_lineNames.size();++i)
        {
            m_plotSource->graph(i)->rescaleValueAxis();
        }
        m_plotSource->yAxis->rescale(true);//y轴值自动变化
        m_plotSource->xAxis->setRange(key_x,8,Qt::AlignRight);
        m_plotSource->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
        m_plotSource->replot();
    }

}

void MyCustomPlotWidget::cusDataAvailable(char value)
{
    double newdata = double(value);
    newDataAvailable(newdata);
}

void MyCustomPlotWidget::onCutMainWindow()
{
    m_serialPlot->show();
}


void MyCustomPlotWidget::upDatePlot(QVector<double> & data)
{
    double mydata = iterationData(data);
    newDataAvailable(mydata);
}

void MyCustomPlotWidget::newSerialDatas(QByteArray data)
{
    double mdata =  data.toDouble();
    newDataAvailable(mdata);
}

void MyCustomPlotWidget::flagfunc(int status)
{
    m_status = status;
//    m_flag = status;
//    if(m_flag == 0)
//    {
//        if(m_customerThread->isRunning())
//        {
//            m_timer->stop();
//            m_timerDynamic->stop();
//            m_customerThread->terminate();
//            qDebug()<<"关闭customerThread线程！";
//        }
//    }
//    else
//    {
//        if(m_serialPlot->mThread.isRunning())
//        {
//            m_timer->start(500);
//            m_timerDynamic->start(20);
//            m_customerThread->start();
//        }
//    }

}

/* */
void MyCustomPlotWidget::on_btnStart_clicked()
{
    if(m_flag == 0 || m_status == 0)
    {
        m_timer->start(500);//定时器
        m_customerThread->start();// 数据处理类 线程开始
        m_timerDynamic->start(20);
    }
    m_flag = 1;
    stopRecive = true;
}


void MyCustomPlotWidget::on_btnPause_clicked()
{
    stopRecive = false;
    if(m_flag == 1 && m_status ==1 )
    {
        m_timer->stop(); //主线程停止发信号,断开数据处理类函数
        m_timerDynamic->stop();//绘图停止
//        m_customerThread->quit();
//        m_customerThread->wait();//阻塞当前线程直到新线程执行完毕，这里是主线程被阻塞，画面卡顿,加一个进度条
        qDebug()<<"关闭customerThread线程！";
    }
    m_flag = 0;

}

void MyCustomPlotWidget::on_btnStop_clicked()
{

//    m_customerThread->wait();
//    //工作线程完成后，析构
//    connect(m_customerThread,&QThread::finished,m_dataprocessing,&QObject::deleteLater);
//    m_customerThread->quit(); //请求线程退出事件循环
//    if(m_customerThread->isRunning())
//    {
//       m_customerThread->terminate();
//    }

}

