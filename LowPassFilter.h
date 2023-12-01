
#ifndef LOWPASSFILTER_H
#define LOWPASSFILTER_H

#include <iostream>
#include <vector>
#include <cmath>
using namespace  std;

//一个一阶滤波器来实现低通滤波器
/*
    该滤波器具有一个截止频率，所有高于该频率的信号成分都会被过滤掉。
    在构造函数中，我们根据采样率和截止频率计算出滤波器的alpha系数,
    这个系数用于控制信号在输入和输出之间的平衡。

    然后，我们在update函数中使用输入信号和alpha系数来计算滤波器的输出，
    并且将输出保存在prev_output_中，
    以便在下一次更新时使用。

    在主函数中，我们创建了一个输入信号向量，并且创建了一个低通滤波器对象。
    然后，我们使用for循环将输入信号中的每个元素传递给滤波器进行处理，并且将输出保存在一个新的向量中。
    最后，我们将滤波后的输出向量输出到控制台。
*/


class LowPassFilter
{

public:
    /**********************************************
    * @projectName   %{CurrentProject: LowPassFilter}
    * @brief         低通滤波器 根据采样率和截止频率计算出滤波器的alpha系数,这个系数用于控制信号在输入和输出之间的平衡。
    * @param         sample_rate :采样率
    * @param         cutoff_frequency:截止频率
    * @return        void
    * @author        ZZX
    * @date          2023-05-16
    **********************************************/

    LowPassFilter(double sample_rate, double cutoff_frequency)
    {
        double dt =1.0/sample_rate;
        double RC = 1.0/(cutoff_frequency*2.0*M_PI);
        alpha_ = dt / (dt + RC); //alpha系数
        prev_output_ = 0.0;
    }

    /**********************************************
    * @projectName   %{CurrentProject:update}
    * @brief         update函数中使用输入信号和alpha系数来计算滤波器的输出
    * @param         input：输入信号值
    * @return        double 返回一个处理后的数据，并且将输出保存在prev_output_中，以便在下一次更新时使用。
    * @author        ZZX
    * @date          2023-05-16
    **********************************************/
    double update(double input)
    {
        double output = alpha_*input + (1.0-alpha_)*prev_output_;
        prev_output_ = output;
        return output;
    }
private:
    double alpha_;
    double prev_output_;

};

// 创建一个函数，用于生成低通滤波器的系数

/**********************************************
* @projectName   %{CurrentProject:createLowpassFilter}
* @brief         createLowpassFilter的函数来生成FIR低通滤波器的系数.
* @param         int M:     滤波器的长度M
* @param         double fc: 截止频率fc
* @param         double fs: 采样率fs
* @return        void
* @author        ZZX
* @date          2023-05-16
**********************************************/





/**********************************************
* @projectName   %{CurrentProject:FirFilter}
* @brief          FIR低通滤波器类
* @param         vector<double>& taps :低通滤波器的系数
* @return        void
* @author        ZZX
* @date          2023-05-16
**********************************************/
// FIR低通滤波器类
class FirFilter
{
public:
    FirFilter(const vector<double>& taps) : taps_(taps), buffer_(taps.size(), 0.0) {}

    // 更新滤波器输出
    double update(double input)
    {
        // 将新的输入添加到环形缓冲区中
        buffer_.insert(buffer_.begin(), input);
        buffer_.pop_back();
        // 计算输出值
        double output = 0.0;
        for (std::size_t i = 0; i < taps_.size(); ++i) {
            output += taps_[i] * buffer_[i];
        }
        return output;
    }

private:
    vector<double> taps_;  // 滤波器系数
    vector<double> buffer_;  // 环形缓冲区
};












#endif // LOWPASSFILTER_H
