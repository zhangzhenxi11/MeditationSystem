
#ifndef MYDATAPROCESSING_H
#define MYDATAPROCESSING_H

#include <QObject>
#include <gsl/gsl_errno.h>       //gsl算法库
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_fft_complex.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_filter.h>
#include <gsl/gsl_rng.h>        // 随机数生成器
#include <gsl/gsl_randist.h>
#include <gsl/gsl_vector.h>
#include "edflib.h"              //edf文件算法库
#include <QDebug>
#include <QFile>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <QThread>              //多线程
#include <Eigen/Dense>          //Eigen数值运算库
#include <windows.h>
#include <QApplication>
#include "comservice.h"
#include "./myfilter/myfilter.h"


/*
 * @brief  对数据处理的类
 * 1. * 子线程1：读取buffer(还做不到)
 * 对来自串口256个数据用buffer存储，每组数据接完，就刷新buffer再进行algo算法处理。
 *
 * 2. 子线程2：读取buffer,保存到文本中。
 * 实时接受串口数据保存到文本中，每256行数据去做FFT算法，得到功率谱数据。
 * 再进行（带通和陷波，去基线偏移算法algo算法处理串口和数据处理类进行通信，producer/customer;
 */

#define  Fs                         250     //采样率
#define  COUNTN                     256     //采样总体个数
#define  FFT_M                      8       // 2^8 = 256
#define  LEN_A_LINE_OF_INPUT_FILE   12      // 输入文件中，每一行的长度
#define REAL(z,i) ((z)[2*(i)])              //z[2i]  z是数组，[]是索引
#define IMAG(z,i) ((z)[2*(i)+1])            //z[2i+1]
#define FILE_DURATION              (600)    // 文件持续时间 600
#define SMP_FREQ                   (250)    // 采样率

//消费者
class MyDataProcessing : public QObject
{
    Q_OBJECT
public:
    explicit MyDataProcessing(QObject *parent = nullptr);
    ~MyDataProcessing();

    /*单例*/
    static MyDataProcessing*GetKernel();

    /* 文本操作函数 */

    /* 保存数据到文本中 */
    void saveEegWaveToOutputFile(double & delta,double &theta,double &alpha,
                                 double &beta, double &gama,const QString & filename);

    /*保存数据到EDF文件中 */
    int saveEegWaveToEdfFile(double & delta,double &theta,double &alpha,
                              double &beta, double &gama,const QString & filename);

    /* 写 */
    void saveDataToFile(QString & filename,QVector<double>clean_array);
    /* 读 */
    void ReadDataToContainer(QString & filename);

    /* 算法函数 */

    //fft算法
    void eegDataProcess(const QString & input_filename,const QString & output_filename);



signals:
    // 滤波后的数据准备好的信号
    void filteredDataReady(QVector<double>& data);
    //发送到主线程的信号
    void newData(char data);

    //发送关注度数值的信号
    void dynamicAttentionData(double attentionData);

private:
    void coeffBandPass(int n, double lowcut, double highcut, int fs,
                       vectord &acof_vec, vectord &bcof_vec);

    void coffStopPass(int n, double lowcut, double highcut, int fs,
                      vectord &ccof_vec, vectord &dcof_vec);

private:
    static MyDataProcessing*     m_dataInstance;
    char                         m_buf[650] = {0};        // 存不同eeg数据
    double                       m_edfBuf[650];           // 存不同eeg数据
    QByteArray                   mBuffer;
    QVector<double>              buffer;                  // 保存滤波后的数据
    QVector<double>              m_originalDatas;         // 用来脑电波原始信号
    QByteArray                   originalBuffer;
    QVector<double>              m_array_x;               // 画图的时候使用，array_x作为横坐标
    ComService *                 serialwidget = nullptr;  //串口类
    QVector<double>              m_savearray;
    double                       m_attention;              //关注值
    gsl_fft_complex_wavetable *  wavetable =nullptr;
    gsl_fft_complex_workspace *  workspace =nullptr;
};

#endif // MYDATAPROCESSING_H

