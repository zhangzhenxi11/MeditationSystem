
#include "mydataprocessing.h"
#include <QFile>

//QMutex mutex;

MyDataProcessing::MyDataProcessing(QObject *parent)
    :QObject{parent}
{
}

MyDataProcessing::~MyDataProcessing()
{
}
void MyDataProcessing::saveEegWaveToOutputFile(double &delta, double &theta, double &alpha, double &beta, double &gama, const QString &filename)
{
    snprintf(m_buf,650,
            "%-30.7f  %-30.7f  %-30.7f  %-30.7f  %-30.7f  \n",
            delta,
            theta,
            alpha,
            beta,
            gama);

    QFile file(filename);
    bool ret = file.open(QIODevice::Append | QIODevice::Text);//字符串
    if(ret)
        file.write(m_buf);
    else
        qDebug()<<QString::fromLocal8Bit("%1:device not open!").arg(filename) << file.errorString();
    file.close();
}

void MyDataProcessing::saveDataToFile(QString &filename, QVector<double> clean_array)
{
    QFile file(QApplication::applicationDirPath()+filename);
    bool ret = file.open(QIODevice::Append | QIODevice::Text);//字符串
    if(ret)
    {
        QTextStream out(&file);
        for(auto it: clean_array)
        {
            out<<it<<"\n" ;
        }
        file.close();
    }
}

void MyDataProcessing::ReadDataToContainer(QString &filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream stream(&file);
        while (!stream.atEnd())
        {
            double buf;
            stream>>buf;
            m_savearray.append(buf);
        }
        file.close();
    }
}

void MyDataProcessing::coeffBandPass(int n, double lowcut, double highcut, int fs, vectord &acof_vec, vectord &bcof_vec)
{
    double nyq = 0.5 * fs;
    double f1f = lowcut / nyq;
    double f2f = highcut / nyq;
    double sf;                 // scaling factor  比例因子
    double *acof;
    int *bcof;
    /* calculate the d coefficients 计算d系数 */
    acof = dcof_bwbp(n, f1f, f2f);
    if (acof == NULL)
    {
        perror("Unable to calculate d coefficients");
    }

    /* calculate the c coefficients 计算c系数 */
    bcof = ccof_bwbp(n);
    if (bcof == NULL)
    {
        perror("Unable to calculate c coefficients");
    }

    sf = sf_bwbp(n, f1f, f2f); /* scaling factor for the c coefficients  c系数的比例因子 */

    /* create the filter coefficient file  创建滤波器系数文件 */
    for (int i = 0; i <= 2 * n; ++i){
        bcof_vec.push_back((double)bcof[i] * sf);
    }

    for (int i = 0; i <= 2 * n; ++i)
        acof_vec.push_back(acof[i]);
}

void MyDataProcessing::coffStopPass(int n, double lowcut, double highcut, int fs, vectord &ccof_vec, vectord &dcof_vec)
{
    double nyq = 0.5 * fs;
    double f1f = lowcut / nyq;
    double f2f = highcut / nyq;
    double sf;
    double *dcof;
    double *ccof;
    /* calculate the d coefficients */
    dcof = dcof_bwbs( n, f1f, f2f );
    if( dcof == NULL )
    {
        perror( "Unable to calculate d coefficients" );
    }

    /* calculate the c coefficients */
    ccof = ccof_bwbs( n, f1f, f2f );
    if( ccof == NULL )
    {
        perror( "Unable to calculate c coefficients" );
    }

    sf = sf_bwbs( n, f1f, f2f ); /* scaling factor for the c coefficients */

    /* create the filter coefficient file  创建滤波器系数文件 */
    for (int i = 0; i <= 2 * n; ++i){
        ccof_vec.push_back(sf*ccof[i]);
    }

    for (int i = 0; i <= 2 * n; ++i)
        dcof_vec.push_back(dcof[i]);
}

void MyDataProcessing::eegDataProcess(const QString &input_filename, const QString & output_filename)
{
    //QFile
    QFile file(input_filename);
    double data[2*N];//数组2N的长度 一维的压缩数组
    double m_EegDataBuf;               //从原始脑电文件中读取每一行数据，保存在这个变量中。
    double m_Temp  = 0.0f;              // 这个临时变量用于保存各个波段的值，双精度浮点型变量
    double m_Delta = 0.0;              // 0~4HZ
    double m_Theta = 0.0;              // 4~8HZ
    double m_Alpha = 0.0;              // 8~12HZ
    double m_Beta  = 0.0;              // 12~30HZ
    double m_Gama  = 0.0;              // 30~100HZ
    QVector<double> m_DataPower;       //功率谱
    if(file.open(QIODevice::ReadOnly))
    {
        while(!file.atEnd())
        {
            QString str = file.readLine();
            m_EegDataBuf = str.toDouble();
            m_originalDatas.append(m_EegDataBuf);
            if(m_originalDatas.size()== N)
            {
                //数组里的复数实部和虚部都置0
                for (int i = 0; i < N; i++)
                {
                    REAL(data,i) = 0.0;//data[2i]=0
                    IMAG(data,i) = 0.0;//data[2i+1]=0
                }
                //已经取满256行，开始准备做FFT运算
                for(int i = 0; i < N; i++)
                {
                    REAL(data,i)= m_originalDatas[i]; //z[2i]        z[0]  z[2]
                    IMAG(data,i) = 0.0;              //z[2i+1]      z[1]  z[3]
                    m_array_x.append(i);
                }
                wavetable = gsl_fft_complex_wavetable_alloc (N);//生成一个 wavetable
                workspace = gsl_fft_complex_workspace_alloc (N);//分配空间
                //傅里叶变换  原位存放
                gsl_fft_complex_forward (data, 1, N,
                                        wavetable, workspace);
                for (int i = 0; i < N; i++)
                {
                    printf ("%d: %e %e\n", i, REAL(data,i),
                           IMAG(data,i));
                }
                int  iLoop = 0;
                //功率谱密度估计,获取各个脑电波频段的功率谱值   只需要取实部，因为虚部为0
                for( iLoop = 0; iLoop<N; iLoop++)
                {
                    //求功率谱，根据FFT计算出来的实部和虚部求模
                    m_DataPower.append((2.0/(Fs*N))*(fabs(REAL(data,iLoop))*fabs(REAL(data,iLoop)) +
                                                           fabs(IMAG(data,iLoop))*fabs(IMAG(data,iLoop))));
                }
                // 计算 delta 波 ，0~4HZ，实际的是1~3HZ
                m_Temp  = 0.0f;
                for( iLoop=1;iLoop<=3;iLoop++)
                {
                    m_Temp += m_DataPower[iLoop];
                }

                m_Delta = m_Temp;

                // 计算 theta 波 ，4~8HZ ，实际的是4~7HZ
                m_Temp = 0.0f;
                for( iLoop=4;iLoop<=7;iLoop++)
                {
                    m_Temp += m_DataPower[iLoop];
                }
                m_Theta = m_Temp;

                // 计算 alpha 波 ，8~12HZ，实际是8~12HZ
                m_Temp = 0.0f;
                for( iLoop=8;iLoop<=12;iLoop++)
                {
                    m_Temp += m_DataPower[iLoop];
                }
                m_Alpha = m_Temp;

                // 计算 beta 波 ，12~30HZ，实际是13~30HZ
                m_Temp = 0.0f;
                for( iLoop=13;iLoop<=30;iLoop++)
                {
                    m_Temp += m_DataPower[iLoop];
                }
                m_Beta = m_Temp;
                // 计算 gama 波 ，30~100HZ，实际测试是31~49HZ，只测试low gama 和middle gama ，因为50HZ处有工频干扰，幅值会很强
                m_Temp = 0.0f;
                for( iLoop=31;iLoop<=49;iLoop++)
                {
                    m_Temp += m_DataPower[iLoop];
                }
                m_Gama = m_Temp;
                m_DataPower.clear();
                m_originalDatas.clear();//清除缓存,才开启下个循环
                //保存到文件中
                saveEegWaveToOutputFile(m_Delta,m_Theta,m_Alpha,m_Beta,m_Gama, output_filename);
                gsl_fft_complex_wavetable_free (wavetable);
                gsl_fft_complex_workspace_free (workspace);
            }/*end if*/
        }/*end while*/

    }
}
