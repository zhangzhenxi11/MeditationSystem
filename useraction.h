#ifndef USERACTION_H
#define USERACTION_H

#include <QWidget>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_complex.h>
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
#include "QTimer"   //定时器
#include <QTime>    //时间
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "LowPassFilter.h"  //低通滤波
#include "bandpassfilter.h" //带通滤波
#include <QThread>          //多线程
#include <Eigen/Dense>      //矩阵库
#include "./myfilter/myfilter.h"
#include "iir.h"
#include "widget.h"

#define  Fs                         250     //采样率
#define  N                          256     //采样总体个数
#define  FFT_M                      8       // 2^8 = 256
#define  LEN_A_LINE_OF_INPUT_FILE   12      // 输入文件中，每一行的长度
#define REAL(z,i) ((z)[2*(i)])              //z[2i]  z是数组，[]是索引
#define IMAG(z,i) ((z)[2*(i)+1])            //z[2i+1]

namespace Ui {
class UserAction;
}

class UserAction : public QWidget
{
    Q_OBJECT

public:
    explicit UserAction(QWidget *parent = nullptr);
    ~UserAction();

signals:
    void timeout();

public:

    //数据更新
    void drawDynamic(QCustomPlot * plot1,int plot_num,QList<QString> _idxList);
    //迭代容器
    double iterationData(QVector<double> &itContainer);

    /* 文本操作函数 */

    /* 把文件中列数据放入容器中 */
    void readColumnData(const QString &filename);
    //
    void saveEegWaveToOutputFile(double & delta,double &theta,double &alpha,
                                double &beta, double &gama, QString & filename);

    void saveDataToFile(QString & filename,QVector<double>clean_array);

    void ReadDataToContainer(QString & filename);

    /* 算法函数 */

    //fft算法
    void eegDataProcess(QString & input_filename,QString & output_filename);

    //高斯滤波器
    void GaussianFilter();


private:

    /**********************************************
    * @projectName   %{CurrentProject:coeff_bandpass}
    * @brief         (带通滤波器系数) 函数计算
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
    void coeffBandPass(int n, double lowcut, double highcut, int fs,
                       vectord &acof_vec, vectord &bcof_vec);

    /**********************************************
    * @projectName   %{CurrentProject:coff_stoppass}
    * @brief         (陷波滤波器系数) 函数计算
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

    void coffStopPass(int n, double lowcut, double highcut, int fs,
                      vectord &ccof_vec, vectord &dcof_vec);

public slots: //声明槽函数

    void realTimeDataSlot();

    void deletePoiter();

    void on_btnStart_clicked();

    void on_btnStop_clicked();

    /* 获取串口类的buffer数据 */
    void newDataAvailable(int value);

private:
    Ui::UserAction *ui;
    gsl_fft_complex_wavetable * wavetable;
    gsl_fft_complex_workspace * workspace;
    QCustomPlot*                plot;               //曲线指针
    QTimer *                    dataTimer;          //定时器
    QList<QString>              idxList;            //当前正在显示的曲线的编号
    int                         flag_start =1;      //标志位 0:停止 1 开始 控制曲线开始与停止

public:

    char m_buf[650] = {0};                  //存不同eeg数据
    char m_serialBuff[256] = {0};           //串口的buffer
    double m_eeg_all_data[256];
    QVector<double> m_array;                //用来脑电波原始信号
    QVector<double> m_array_x;              //画图的时候使用，array_x作为横坐标
    QVector<double>  m_filt_all_eeg;        //滤波后的全部数据
    QVector<double> savearray;
    QVector<double> gaussian_y1;            //高斯滤波测试
    QVector<double> gaussian_y2;
    QVector<double> gaussian_y3;
    QVector<double> gaussian_x;
    QVector<double>DeltaVect,ThetaVect,AlphaVect,
                   BetaVect,GamaVect, OtherVect; //不同波形数据容器
    QMap<QString,QVector<double>> EegContainer ;
    QStringList                   EegNameList;
    QVector<double>               input_signal; //低通滤波测试
    QVector<double>               output_signal;//低通滤波测试
    QVector<double>               FIR_signal;
    QVector<double>               BAND_signal;
    QList<QString>                mlineNames;   //曲线名称
    QVector<QCPGraphData>         m_timeData;
    double  m_value;

};

#endif // USERACTION_H
