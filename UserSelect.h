
#ifndef USERSELECT_H
#define USERSELECT_H

#include <QMainWindow>
#include<gsl/gsl_errno.h>
#include<gsl/gsl_fft_complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_filter.h>
#include <gsl/gsl_rng.h>  // 随机数生成器
#include <gsl/gsl_randist.h>
#include <gsl/gsl_vector.h>
#include <math.h>
#include "qcustomplot.h" //绘制曲线图
#include <complex.h>
#include <QDebug>
#include <QFile>
#include "QTimer"//定时器
#include <QTime> //时间
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

#include "LowPassFilter.h"

#include "bandpassfilter.h"

#include "myMultiCurvesPlot.h"

#include <QThread>  //多线程

#include <Eigen/Dense>//矩阵库

#include "./myfilter/myfilter.h"

#include "iir.h"

QT_BEGIN_NAMESPACE
namespace Ui { class UserSelect; }
QT_END_NAMESPACE

#define  Fs                         250     //采样率
#define  N                          256     //采样总体个数
#define  FFT_M                      8      // 2^8 = 256
#define  LEN_A_LINE_OF_INPUT_FILE   12     // 输入文件中，每一行的长度
#define REAL(z,i) ((z)[2*(i)])//z[2i]  z是数组，[]是索引
#define IMAG(z,i) ((z)[2*(i)+1])//z[2i+1]


/*
GSL中的fft和自定义数字滤波  UserSelect
*/

class UserSelect : public QWidget

{
    Q_OBJECT

public:
    UserSelect(QWidget *parent = nullptr);
    ~UserSelect();
private:
    Ui::MainWindow *ui;

signals:
    void timeout();

public:
    //初始化主界面
//    void initWidget();

    //数据更新
    void drawDynamic(QCustomPlot * plot1,int plot_num,QList<QString> _idxList);
    //迭代容器
    double iteration_data(QVector<double> &itContainer);

    //把文件中列数据放入容器中
    void readcolumndata(const QString &filename);

    void save_eeg_wave_to_OutputFile(double & delta,double &theta,double &alpha,
                                     double &beta, double &gama, QString & filename);
    //fft算法
    void eeg_data_process(QString & input_filename,QString & output_filename);

    void eeg_data_proc_task();

    void save_data(QString & filename,QVector <double> clean_array);

    void ReadLine(QString & filename);

    //高斯滤波器
    void Gaussian_filter();
    void eigen_test();

    //fir测试生成低通滤波器的系数
    vector<double> createLowpassFilter(int M, double fc, double fs);

    /**********************************************
    * @projectName   %{CurrentProject:coeff_bandpass}
    * @brief         bandpass filter coefficient (带通滤波器系数) 函数计算
    *                数字截至频率公式：2*f/fs f:是截止频率，或者是上/下截止频率  fs:采样率
    * @param         n:  filter order
    * @param         lowcut: lower cutoff frequency (fraction of pi)
    * @param         highcut: upper cutoff frequency (fraction of pi)
    * @param         fs: 采样率
    * @param         acof_vec: d coefficients
    * @param         bcof_vec: c coefficients
    * @return        void
    * @author        ZZX
    * @date          2023-05-19
    **********************************************/
    void coeff_bandpass(int n, double lowcut, double highcut, int fs,vectord &acof_vec, vectord &bcof_vec);

    /**********************************************
    * @projectName   %{CurrentProject:coff_stoppass}
    * @brief         stoppass filter coefficient (陷波滤波器系数) 函数计算
    * @param         n:  filter order
    * @param         lowcut: lower cutoff frequency (fraction of pi)
    * @param         highcut: upper cutoff frequency (fraction of pi)
    * @param         fs: 采样率
    * @param         ccof_vec: d coefficients
    * @param         dcof_vec: c coefficients
    * @return        void
    * @author        ZZX
    * @date          2023-05-19
    **********************************************/

    void coff_stoppass(int n, double lowcut, double highcut, int fs,vectord &ccof_vec, vectord &dcof_vec);

public slots: //声明槽函数
    void realTimeDataSlot();
    void delete_poiter();

private:
    gsl_fft_complex_wavetable * wavetable;
    gsl_fft_complex_workspace * workspace;
    QCustomPlot* plot;
//    QThread* t1Thread;
    QTimer *dataTimer;

public:
    char m_buf[650] = {0};             //存不同eeg数据
    double m_eeg_all_data[256];
    QVector<double> m_array;           //用来脑电波原始信号
    QVector<double> m_array_x;         //画图的时候使用，array_x作为横坐标
    QVector<double>  m_filt_all_eeg;    //滤波后的全部数据
    QVector<double> savearray;


    QVector<double> gaussian_y1;           //高斯滤波测试
    QVector<double> gaussian_y2;
    QVector<double> gaussian_y3;
    QVector<double> gaussian_x;

    QVector<double> DeltaVect, ThetaVect,AlphaVect,BetaVect,GamaVect,OtherVect;

    QMap<QString,QVector<double>> EegContainer ;
    QStringList                   EegNameList;

    QVector<double>  input_signal; //低通滤波测试
    QVector<double>  output_signal;//低通滤波测试
    QVector<double>   FIR_signal;
    QVector<double>   BAND_signal;

    QList<QString> mlineNames;
    QVector<QCPGraphData>   m_timeData;

private slots:
    void on_btnStart_clicked();

    void on_btnStop_clicked();


private:
//    QVector<QColor> getColor;  //记录所有曲线的颜色
//    QVector<QString> getName;  //记录所有曲线的名称（图例）
    QList<QString> idxList;  //当前正在显示的曲线的编号
    int flag_start=1;//标志位 0:停止 1 开始 控制曲线开始与停止
};

#endif // USERSELECT_H
