
#ifndef MYFILTER_H
#define MYFILTER_H

#include <iostream>
#include <exception>
#include <algorithm> //C++ STL标准中的基础性的算法库（均为函数模板）
#include "Eigen/Dense"//矩阵运算数学库

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h> //数学库
#include "float.h"
#include "iir.h"
typedef std::vector<int> vectori;
typedef std::vector<double> vectord;

using namespace Eigen;
using namespace std;


/*
例子：
int fs = 600000;//采样频率

vectord cof_a,cof_b;

coeff_ab(1, 200000, 250000, fs, cof_a, cof_b);// coefficients of b,a

filtfilt(cof_b, cof_a, inputdata, outputdata);

*/

class Myfilter
{
public:
    Myfilter();

    ~Myfilter();

    void add_index_range(vectori &indices, int beg, int end, int inc = 1);

    void add_index_const(vectori &indices, int value, size_t numel);

    //vec容器中插入tail容器
    void append_vector(vectord &vec, const vectord &tail);

    //向量反向并返回
    vectord subvector_reverse(const vectord &vec, int idx_end, int idx_start);

    //返回范围内的最大元素
    inline int max_val(const vectori& vec);

    //零相位滤波
    void filtfilt(vectord B, vectord A, const vectord &X, vectord &Y);

    //滤波函数
    void filter(vectord B, vectord A, const vectord &X, vectord &Y, vectord &Zi);

    //比较
    bool compare(const vectord &original, const vectord &expected,double tolerance = DBL_EPSILON);

};

#endif // MYFILTER_H
