
#ifndef BANDPASSFILTER_H
#define BANDPASSFILTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <iostream>
#include <vector>
#include <QVector>
#include <exception>
#include <algorithm>

/*
 * 以下是定时器采用到的宏
 */
#define NUM_OF_SAMPLE_PER_SECOND   256     /* 定时器每秒采样脑电数据256次 */
#define EEG_ALL_DATA_SIZE          NUM_OF_SAMPLE_PER_SECOND

/*
 * 以下是task 使用到的宏
 */
#define PEAKS_VALLY_MAX_NUMS  5         /* 峰和谷的最大个数 */
#define RANK_OF_CHEBY_FILT    5         /* 切比雪夫滤波器的阶数 */
#define FILT_EPS              0.000001

//传统滤波（如Matlab的filter函数）会造成信号的延迟，延迟程度与滤波器的阶次有关，
//为了解决延迟问题，Matlab提供了filtfilt函数，该方法一般称为零相位滤波或双向滤波。

/* SuGaomin 2020-08-29
 *
 * 第一，double变量占8个字节，一般后面保留15位小数，你这个小数后面太长了
 * 第二，double类型的数，在小数后面需要加上一个d或者D，
 *      float类型后面的数，需要加一个f
 *      long 类型的，需要加一个L
 * 第三，这个PI应该是个常量，在系统里面是有的，不需要自己额外的定义。
 *
 * 可能你这个代码是之前拷贝我的。我的代码里面是当时在C环境下运行的。我的环境下是没有的，所以需要我自己定义。
*/


class BandPassFilter
{
public:
    BandPassFilter();
    ~BandPassFilter();

    // filter滤波器函数，最终被filtfilt 调用
    void filter(const double* x,
                double      * y,
                int           xlen,
                double      * a,
                double      * b,
                int           nfilt,    /* nfilt为系数数组长度 */
                double      * zi);

    /*
     * 矩阵乘法，最终被filtfilt 调用
     * 矩阵乘法  m为a的行数，n为a的列数数，k为b的行数，第一个矩阵列数必须要和第二个矩阵的行数相同
     */
    void trmul(double *a,
               double *b,
               double *c,
               int     m,
               int     n,
               int     k);

    /*
     * 求逆矩阵，最终被filtfilt 调用
     * 求逆矩阵，当返回值为0时成功，a变为逆矩阵
     */
    int rinv(double *a,int n);

    //
    /* filtfilt函数
     * MATLAB : t=filtfilt(b,a,x);
     * x input array
     * y output array
     * xlen ，sizeof(x)
     * a
     * b
     * nfilt,sizeof(a) || sizeof(b)
     */
    int filtfilt(double* x,
                 double* y,
                 int xlen,
                 double* a,
                 double* b,
                 int nfilt);

    //用于处理采集到的脑电数据
    void eeg_data_proc_task(double(*arr_raw_data)[256]); // double arr[250]

private:
    /* 滤波及一阶差分后的脑电数据 */
    double   eeg_raw_data[EEG_ALL_DATA_SIZE] =  {0.0}; //input array
    double   filt_eeg_data[EEG_ALL_DATA_SIZE] = {0.0};
    int      g_eeg_all_data[EEG_ALL_DATA_SIZE] = {0};
};

using namespace std;

/**********************************************
* @projectName   %{CurrentProject:MyBandPassFilter}
* @brief         FIR带通滤波器
* @return        void
* @author        ZZX
* @date          2023-05-16
**********************************************/
class MyBandPassFilter{

public:
    MyBandPassFilter();

    ~MyBandPassFilter();
/**********************************************
* @projectName   %{CurrentProject:bandpass_filter}
* @brief         对于一个给定的阶数 num_taps，h 数组的值通过选择一个带通区域来计算。
*                filtered_signal 数组的值是通过将信号与滤波器系数进行卷积来计算的。
* @param         signal:输入信号
* @param         f1:截止频率
* @param         f2:截止频率
* @param         fs:采样频率
* @param         num_taps:滤波器的阶数（即系数数量）
* @return        void
* @author        ZZX
* @date          2023-05-16
**********************************************/

QVector<double> bandPassFilter(QVector<double>& signal, double f1, double f2, double fs, int num_taps);
private:
    int n ;          //信号数组大小
    QVector<double> h;//滤波器系数
    QVector<double> filtered_signal;//存储滤波后的信号
};

#endif // BANDPASSFILTER_H
