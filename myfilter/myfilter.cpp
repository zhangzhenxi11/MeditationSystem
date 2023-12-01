
#include "myfilter.h"

Myfilter::Myfilter()
{

}

Myfilter::~Myfilter()
{

}

/* SuGaomin 2020-08-29
*
* 下面两个函数，for循环和while循环，循环语句没有加{}
* add_index_range
* add_index_const
*/
void Myfilter::addIndexRange(vectori &indices, size_t beg, size_t end, size_t inc)
{
    for (size_t i = beg; i <= end; i += inc)
    {
        indices.push_back(i);
    }

}

void Myfilter::addIndexConst(vectori &indices, size_t value, size_t numel)
{
    while (numel--)
    {
        indices.push_back(value);
    }

}

void Myfilter::appendVector(vectord &vec, const vectord &tail)
{
    vec.insert(vec.end(), tail.begin(), tail.end());
}

vectord Myfilter::subVectorReverse(const vectord &vec, size_t idxEnd, size_t idxStart)
{
    vectord result(&vec[idxEnd], &vec[idxStart + 1]);
    std::reverse(result.begin(), result.end());//反转 [first, last) 范围中的元素顺序
    return result;
}

int Myfilter::maxVal(const vectori &vec)
{
    return std::max_element(vec.begin(), vec.end())[0];
}

void Myfilter::filtFilt(vectord B, vectord A, const vectord &X, vectord &Y)
{
    int len = X.size();     // length of input
    int na = A.size();
    int nb = B.size();
    int nfilt = (nb > na) ? nb : na;
    int nfact = 3 * (nfilt - 1); // length of edge transients

    if (len <= nfact)
        throw std::domain_error("Input data too short! Data must have length more than 3 times filter order.");

    // set up filter's initial conditions to remove DC offset problems at the
    // beginning and end of the sequence
    B.resize(nfilt, 0);
    A.resize(nfilt, 0);

    vectori rows, cols;
    //rows = [1:nfilt-1           2:nfilt-1             1:nfilt-2];
    addIndexRange(rows, 0, nfilt - 2);
    if (nfilt > 2)
    {
        addIndexRange(rows, 1, nfilt - 2);
        addIndexRange(rows, 0, nfilt - 3);
    }
    //cols = [ones(1,nfilt-1)         2:nfilt-1          2:nfilt-1];
    addIndexConst(cols, 0, nfilt - 1);
    if (nfilt > 2)
    {
        addIndexRange(cols, 1, nfilt - 2);
        addIndexRange(cols, 1, nfilt - 2);
    }
    // data = [1+a(2)         a(3:nfilt)        ones(1,nfilt-2)    -ones(1,nfilt-2)];

    auto klen = rows.size();
    vectord data;
    data.resize(klen);
    data[0] = 1 + A[1];  int j = 1;
    if (nfilt > 2)
    {
        /* SuGaomin 2020-08-29
         *
         * 所有的循环语句，条件判断语句，必须加{}
         * 否则，别人用新的文本阅读器，就可能导致你的文件缩进错位。
         * 导致代码逻辑错误。
        */
        for (int i = 2; i < nfilt; i++)
        {
            data[j++] = A[i];
        }
        for (int i = 0; i < nfilt - 2; i++)
        {
            data[j++] = 1.0;
        }
        for (int i = 0; i < nfilt - 2; i++)
        {
            data[j++] = -1.0;
        }
    }

    vectord leftpad = subVectorReverse(X, nfact, 1);
    double _2x0 = 2 * X[0];
    std::transform(leftpad.begin(), leftpad.end(), leftpad.begin(), [_2x0](double val) {return _2x0 - val; });

    vectord rightpad = subVectorReverse(X, len - 2, len - nfact - 1);
    double _2xl = 2 * X[len - 1];
    std::transform(rightpad.begin(), rightpad.end(), rightpad.begin(), [_2xl](double val) {return _2xl - val; });

    double y0;

    /* SuGaomin 2020-08-29
    *
    * 我跟潘秋平聊的时候，确认，你这个代码里面的signal1 以前定义成signal，signal是关键词，会自动高亮显示的。
    * 注意不能使用关键词作为变量。其次，不能这么随意定义变量。signal1和signal2的区别是什么？
    * 你自己现在还记得清吗？你是不是需要再阅读一下代码才能了解一下signal1和signal2的区别。
    *
    * 其次，所有的变量都定义在函数开头。即，遵循，先定义后使用的规则。
    * 虽然，现在C++的标准，允许，在原文后面进行变量的定义。
    * 但是，要确保有些平台环境没有支持高版本的C++标准
    */

    vectord signal1, signal2, zi;

    signal1.reserve(leftpad.size() + X.size() + rightpad.size());
    appendVector(signal1, leftpad);
    appendVector(signal1, X);
    appendVector(signal1, rightpad);

    // Calculate initial conditions
    MatrixXd sp = MatrixXd::Zero(maxVal(rows) + 1, maxVal(cols) + 1);
    for (size_t k = 0; k < klen; ++k)
    {
        sp(rows[k], cols[k]) = data[k];
    }
    auto bb = VectorXd::Map(B.data(), B.size());
    auto aa = VectorXd::Map(A.data(), A.size());
    MatrixXd zzi = (sp.inverse() * (bb.segment(1, nfilt - 1) - (bb(0) * aa.segment(1, nfilt - 1))));
    zi.resize(zzi.size());

    // Do the forward and backward filtering
    y0 = signal1[0];
    std::transform(zzi.data(), zzi.data() + zzi.size(), zi.begin(), [y0](double val) { return val*y0; });
    filter(B, A, signal1, signal2, zi);
    std::reverse(signal2.begin(), signal2.end());
    y0 = signal2[0];
    std::transform(zzi.data(), zzi.data() + zzi.size(), zi.begin(), [y0](double val) { return val*y0; });
    filter(B, A, signal2, signal1, zi);
    Y = subVectorReverse(signal1, signal1.size() - nfact - 1, nfact);
}

void Myfilter::filter(vectord B, vectord A, const vectord &X, vectord &Y, vectord &Zi)
{
    if (A.empty())
    {
        throw std::domain_error("The feedback filter coefficients are empty.");
    }
    if (std::all_of(A.begin(), A.end(), [](double coef) { return coef == 0; }))
    {
        throw std::domain_error("At least one of the feedback filter coefficients has to be non-zero.");
    }
    if (A[0] == 0)
    {
        throw std::domain_error("First feedback coefficient has to be non-zero.");
    }

    // Normalize feedback coefficients if a[0] != 1;
    auto a0 = A[0];
    if (a0 != 1.0)
    {
        std::transform(A.begin(), A.end(), A.begin(), [a0](double v) { return v / a0; });
        std::transform(B.begin(), B.end(), B.begin(), [a0](double v) { return v / a0; });
    }

    size_t input_size = X.size();
    size_t filter_order = std::max(A.size(), B.size());
    B.resize(filter_order, 0);
    A.resize(filter_order, 0);
    Zi.resize(filter_order, 0);
    Y.resize(input_size);

    const double *x = &X[0];
    const double *b = &B[0];
    const double *a = &A[0];
    double *z = &Zi[0];
    double *y = &Y[0];

    /* SuGaomin 2023-08-29
     *
     * 凡是有两层循环，及两层循环以上的，里面的循环，必须独立成子函数。进行参数传递调用
     * 降低圈复杂度
     *
    */
    for (size_t i = 0; i < input_size; ++i)
    {
        size_t order = filter_order - 1;
        while (order)
        {
            if (i >= order)
                z[order - 1] = b[order] * x[i - order] - a[order] * y[i - order] + z[order];
            --order;
        }
        y[i] = b[0] * x[i] + z[0];
    }
    Zi.resize(filter_order - 1);
}


/* SuGaomin 2023-08-29
 *
 * 第一，我没看到compare这个函数在哪里被调用到。如果你确实有调用该语句的地方，你自己查找一下
 * 第二，浮点型的数据是怎么比较的？你自己上网查一下。
 * if ((x>=-EPSINON) && (x<=EPSINON))
   其中EPSINON 是允许的误差（即精度）
 *
*/
bool Myfilter::compare(const vectord &original, const vectord &expected,double tolerance)
{
    if (original.size() != expected.size())
    {
        return false;
    }

    /* SuGaomin 2023-08-29
     *
     * 这个size的变量有存在的必要吗？？？？精简代码
     *
    */
    size_t size = original.size();

    for (size_t i = 0; i < size; ++i)
    {
        auto diff = abs(original[i] - expected[i]);
        if (diff >= tolerance)
        {
            return false;
        }
    }
    return true;
}

