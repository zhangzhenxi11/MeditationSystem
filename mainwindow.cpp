
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::testfft()
{
    int N = 2048;
    fftw_complex *in, *out;
    fftw_plan p;
    in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    QVector<double> in_1,in_2;
    int index=0;
    double max=0;
    int n;
    for (n = 0; n < N; n++)
    {
        in[n][1] = 10*sin(40*3.1415926*n/800);
        in[n][0] = 10*cos(40*3.1415926*n/800);
        in_1<<in[n][1];
        in_2<<in[n][0];
    }

    fftw_execute(p);
    fftw_destroy_plan(p);

    QVector<double> x1,y1;

    for (n = 0; n < N; n++)
    {
        //printf("%3.2lf+%3.2lfi    ", out[n][0], out[n][1]);
        x1<<n;
        //y1<<out[n][0];
        double value = 10*log10(out[n][0]*out[n][0]+out[n][1]*out[n][1]);
        y1<<value;
        if(value >max) max = value;
    }
    for(n = 0; n < N/2; n++)
    {
        int i = n, j = N/2+n;
        double temp = 0;
        temp = y1[i];
        y1[i] = y1[j];
        y1[j] = temp;
        if(y1[i] == max) index = i;
        if(y1[j] == max) index = j;
    }
    printf("\n");
    fftw_free(in);
    fftw_free(out);

    //画图
    ui->qcustomplot->graph(0)->setData(x1,y1);
    ui->qcustomplot2->graph(0)->setData(x1,in_1);
    ui->qcustomplot2->graph(1)->setData(x1,in_2);

    QCPGraph* graph = ui->qcustomplot->graph(0);
    AxisTag * mTag1 = new AxisTag(graph->valueAxis());
    mTag1->setPen(graph->pen());
    mTag1->updatePosition(index,max);
    mTag1->setText(QString::number(max, 'f', 2));
    ui->qcustomplot->replot();
    ui->qcustomplot2->replot();
}

void MainWindow::customPlot_init()
{
    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(50, 50, 50));
    plotGradient.setColorAt(1, QColor(50, 50, 50));

    ui->qcustomplot->setBackground(plotGradient);
    ui->qcustomplot->xAxis->setTickLabelColor(Qt::white);
    ui->qcustomplot->yAxis->setTickLabelColor(Qt::white);
    ui->qcustomplot->xAxis->setBasePen(QPen(QColor(140, 140, 140),1));
    ui->qcustomplot->yAxis->setBasePen(QPen(QColor(140, 140, 140),1));
    ui->qcustomplot->xAxis2->setBasePen(QPen(QColor(140, 140, 140),1));
    ui->qcustomplot->yAxis2->setBasePen(QPen(QColor(140, 140, 140),1));
    ui->qcustomplot->xAxis2->setVisible(true);
    ui->qcustomplot->xAxis2->setTickLabels(false);
    ui->qcustomplot->yAxis2->setVisible(true);
    ui->qcustomplot->yAxis2->setTickLabels(false);

    ui->qcustomplot->addGraph();
    ui->qcustomplot->graph(0)->setPen(QPen(Qt::green));
    //ui->qcustomplot->graph(0)->setData(data_x, data_y);

    //    ui->qcustomplot->addGraph();
    //    ui->qcustomplot->graph(1)->setPen(QPen(Qt::yellow));
    //ui->qcustomplot->graph(1)->setData(data_x, data_y2);

    double xAxis_Max = 2100;
    double xAxis_Min = 0;
    double yAxis_Max = 100;
    double yAxis_Min = 0;

    ui->qcustomplot->xAxis->setRange(xAxis_Min,xAxis_Max);
    ui->qcustomplot->yAxis->setRange(yAxis_Min,yAxis_Max);
    ui->qcustomplot->setInteractions(QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectItems );
    ui->qcustomplot->setSelectionRectMode(QCP::srmZoom);
//    ui->qcustomplot->showTracer(true);

}


