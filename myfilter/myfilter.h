
#ifndef MYFILTER_H
#define MYFILTER_H

#include <iostream>
#include <exception>


/* SuGaomin 2020-08-29
 *
 * 这里提到的：//C++ STL标准中的基础性的算法库（均为函数模板）
 * 首先，C++ STL标准目前都在迭代更新。该标准版本号也在迭代更新
 *
 * 需要明确一下，该算法库是不是支持所有的通用STL标准。有没有版本兼容性限制。
 * 否则，在某些老的平台上面，可能你的程序就不能运行。
 *
 * 尽量避免采用特殊标准库的文件
 *
 * 假设这样一个例子：STL 标准11.0版本中支持A函数
 * STL 17.0版本中，把A函数替换成了B函数调用，A函数作为一个废弃接口。
 * 那么你的代码在新老平台上运行就会出现问题。尽量避免废弃接口的使用
*/
#include <algorithm> //C++ STL标准中的基础性的算法库（均为函数模板）
#include "Eigen/Dense"//矩阵运算数学库
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* SuGaomin 2020-08-29
 *
 * <math.h> 和 <algorithm>库是否重复？
 * <algorithm> 这个库应该是C++里面的库
 * <math.h> 这个库是C语言里面的库
 *
 * 避免引入一些不必要的头文件，导致自己的工程代码编译后体积变大。
 * 重复的头文件应该及时去除
 *
 */

#include <math.h> //数学库

/* SuGaomin 2020-08-29
 *
 * "float.h"
 * 这个头文件很危险啊，这个头文件很像C语言头文件。这种头文件的命名需要规范
 *
 */

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

    /* SuGaomin 2020-08-29
     *
     * 自定义的函数命名遵循QT的风格，采用驼峰命名法，后续所有的文件和函数都更改过来
     *
     */
    void addIndexRange(vectori &indices, size_t beg, size_t end, size_t inc = 1);

    /* SuGaomin 2020-08-29
     *
     * 为什么这个函数里面size_t和int 混用？？？可以统一成size_t
     *
     */
    void addIndexConst(vectori &indices, size_t value, size_t numel);

    //vec容器中插入tail容器
    void appendVector(vectord &vec, const vectord &tail);

    //向量反向并返回
    vectord subVectorReverse(const vectord &vec, size_t idxEnd, size_t idxStart);

    //返回范围内的最大元素
    inline int maxVal(const vectori& vec);

    //零相位滤波
    void filtFilt(vectord B, vectord A, const vectord &X, vectord &Y);

    //滤波函数
    void filter(vectord B, vectord A, const vectord &X, vectord &Y, vectord &Zi);

    //比较
    bool compare(const vectord &original, const vectord &expected,double tolerance = DBL_EPSILON);

};

/* SuGaomin 2020-08-29
 *
 * 所有// 注释的都需要替换
 *
 */

#endif // MYFILTER_H
