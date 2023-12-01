
#include "mydataprocessing.h"
#include <QFile>
MyDataProcessing *MyDataProcessing::m_dataInstance = nullptr;
MyDataProcessing::MyDataProcessing(QObject *parent)
    :QObject{parent}
{

}

MyDataProcessing::~MyDataProcessing()
{
}

MyDataProcessing *MyDataProcessing::GetKernel()
{
    if(m_dataInstance==nullptr)
    {
        m_dataInstance= new MyDataProcessing();
    }
    return m_dataInstance;
}

void MyDataProcessing::DestoryKernel()
{
    if(m_dataInstance)
    {
        delete m_dataInstance;
        m_dataInstance = nullptr;
    }

}
void MyDataProcessing::saveEegWaveToOutputFile(double &delta, double &theta, double &alpha,
                                               double &beta, double &gama, const QString &filename)
{
    double meditation = 100 * (alpha)/(alpha + theta + 0.000000001); /* 冥想度 */
    m_attention       = 100 - meditation;                            /* 关注度 */
    emit dynamicAttentionData(m_attention); //发送关注度数值
    snprintf(m_buf,650,
            "%-30.7f  %-30.7f  %-30.7f  %-30.7f  %-30.7f  \n",
            delta,
            theta,
            alpha,
            beta,
            gama,
            m_attention);

    QFile file(filename);
    bool ret = file.open(QIODevice::Append | QIODevice::Text);//字符串
    if(ret)
        file.write(m_buf);
    else
        qDebug()<<QString::fromLocal8Bit("%1:device not open!").arg(filename) << file.errorString();
    file.close();
}

int MyDataProcessing::saveEegWaveToEdfFile(double &delta, double &theta, double &alpha,
                                           double &beta, double &gama, const QString &filename)
{
    m_edfBuf[650];
    double meditation = 100 * (alpha)/(alpha + theta + 0.000000001); /* 冥想度 */
    m_attention       = 100 - meditation;                            /* 关注度 */
    emit dynamicAttentionData(m_attention); //发送关注度数值
    char* chlableList[6] = {"delta" ,"theta","alpha","beta","gamma","attention"};
    int i,     //通道计数
        j,     //文件时长计数
        hdl,   //句柄
        chns=6;//通道数
    // 1.创建一个文件
    QByteArray arrPath = filename.toLocal8Bit();
    char* path = arrPath.data();
    hdl = edfopen_file_writeonly(path, EDFLIB_FILETYPE_EDFPLUS, chns);
    if(hdl<0)
    {
        printf("error: edfopen_file_writeonly()\n");

        return(1);
    }
    //2.2 设置参数
    //1. 设置各通道采样频率
    for (int i = 0; i < chns; ++i)
    {
        if(edf_set_samplefrequency(hdl,i,Fs))
        {
            printf("error: edf_set_samplefrequency()\n");
            return(1);
        }
    }
    //2.设置各通道数字范围（digital）
    for (int i = 0; i < chns; ++i)
    {
        if(edf_set_digital_maximum(hdl, i, 32767))//EDF最大支持16位 BDF支持24位
        {
            printf("error: edf_set_digital_maximum()\n");

            return(1);
        }
    }

    for (int i = 0; i < chns; ++i)
    {
        if(edf_set_digital_minimum(hdl, i, -32768))//EDF最大支持16位 BDF支持24位
        {
            printf("error: edf_set_digital_minimum()\n");

            return(1);
        }
    }

    //3.设置各通道物理范围
    for(i=0; i<chns; i++)
    {
        if(edf_set_physical_maximum(hdl, i, 1000.0))
        {
            printf("error: edf_set_physical_maximum()\n");

            return(1);
        }
    }
    for(i=0; i<chns; i++)
    {
        if(edf_set_physical_minimum(hdl, i, -1000.0))
        {
            printf("error: edf_set_physical_minimum()\n");

            return(1);
        }
    }

    //4.设置各通道物理单位 mV的平方
    for(i=0; i<chns; i++)
    {
        if(edf_set_physical_dimension(hdl, i, "mV^2"))
        {
            printf("error: edf_set_physical_dimension()\n");

            return(1);
        }
    }

    //5.设置各通道lable
    for(i=0; i<chns; i++)
    {
        if(edf_set_label(hdl,i,chlableList[i]))
        {
            printf("error: edf_set_label()\n");
            return(1);
        }
    }

    //6、设置被试信息
    //姓名、性别、生日、编号
    //参数 map 保存受试者、操作员基本信息
    edf_set_patientname (hdl,"张杰");
    edf_set_sex(hdl,1);//1 is male, 0 is female
    edf_set_birthdate(hdl, 1969, 6, 30);//设置生日
    edf_set_patientcode(hdl,"1");

    //7、操作员信息
    edf_set_technician(hdl,"李四");

    //8、写入设备信息
    if(edf_set_equipment(hdl, "EEG generator"))
    {
        printf("edf_set_equipment()\n");

        return(1);
    }
    //9、按顺序写入数据
        //buf赋值
//        delta = 0,
//        theta = 0,
//        alpha = 0,
//        beta = 0,
//        gama = 0,
//        attention = 0;
    //模拟数据，测试用的


    for(j=0; j< FILE_DURATION; j++)//文件时长是600秒
    {
        for(i=0; i<Fs; i++)                /* delta */
        {
            m_edfBuf[i] = delta;
        }
        if(edfwrite_physical_samples(hdl, m_edfBuf))
        {
            printf("error: edfwrite_physical_samples()\n");
            return(1);
        }

        for(i=0; i<Fs; i++)                /* theta */
        {
            m_edfBuf[i] = theta;
        }
        if(edfwrite_physical_samples(hdl, m_edfBuf))
        {
            printf("error: edfwrite_physical_samples()\n");
            return(1);
        }

        for(i=0; i<Fs; i++)                /* alpha */
        {
            m_edfBuf[i] = alpha;
        }
        if(edfwrite_physical_samples(hdl, m_edfBuf))
        {
            printf("error: edfwrite_physical_samples()\n");
            return(1);
        }

        for(i=0; i<Fs; i++)                /* beta */
        {
            m_edfBuf[i] = beta;
        }
        if(edfwrite_physical_samples(hdl, m_edfBuf))
        {
            printf("error: edfwrite_physical_samples()\n");
            return(1);
        }

        for(i=0; i<Fs; i++)                /* gamma */
        {
            m_edfBuf[i] = gama;
        }
        if(edfwrite_physical_samples(hdl, m_edfBuf))
        {
            printf("error: edfwrite_physical_samples()\n");
            return(1);
        }

        for(i=0; i<Fs; i++)                /* attention */
        {
            m_edfBuf[i] = m_attention;
        }
        if(edfwrite_physical_samples(hdl, m_edfBuf))
        {
            printf("error: edfwrite_physical_samples()\n");
            return(1);
        }
    }

    //10、加入事件
    /*将注释/事件写入文件。 此函数是可选的，只能在以写模式打开文件之后和关闭文件之前调用。*/
    edfwrite_annotation_latin1_hr(hdl, 0LL, -1LL, "Recording starts");
    edfwrite_annotation_latin1_hr(hdl, 298000000LL, -1LL, "Test 1");
    edfwrite_annotation_latin1_hr(hdl, FILE_DURATION * 1000000LL, -1LL, "Recording ends");
    //11、关闭文件
    edfclose_file(hdl);
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

void MyDataProcessing::coeffBandPass(int n, double lowcut, double highcut,
                                     int fs, vectord &acof_vec, vectord &bcof_vec)
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

void MyDataProcessing::coffStopPass(int n, double lowcut, double highcut,
                                    int fs, vectord &ccof_vec, vectord &dcof_vec)
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
    qInfo()<< __FUNCTION__ << QThread::currentThreadId();
    //QFile
    QFile file(input_filename);
    double data[2*COUNTN];//数组2N的长度 一维的压缩数组
    double m_EegDataBuf;               //从原始脑电文件中读取每一行数据，保存在这个变量中。
    double m_Temp  = 0.0f;             // 这个临时变量用于保存各个波段的值，双精度浮点型变量
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
            if(m_originalDatas.size()==COUNTN)
            {
                //数组里的复数实部和虚部都置0
                for (int i = 0; i < COUNTN; i++)
                {
                    REAL(data,i) = 0.0;//data[2i]=0
                    IMAG(data,i) = 0.0;//data[2i+1]=0
                }
                //已经取满256行，开始准备做FFT运算
                for(int i = 0; i < COUNTN; i++)
                {
                    REAL(data,i)= m_originalDatas[i]; //z[2i]       z[0]  z[2]
                    IMAG(data,i) = 0.0;              //z[2i+1]      z[1]  z[3]
                    m_array_x.append(i);
                }
                wavetable = gsl_fft_complex_wavetable_alloc (COUNTN);//生成一个 wavetable
                workspace = gsl_fft_complex_workspace_alloc (COUNTN);//分配空间
                //傅里叶变换  原位存放
                gsl_fft_complex_forward (data, 1, COUNTN,
                                        wavetable, workspace);
                for (int i = 0; i < COUNTN; i++)
                {
                    printf ("%d: %e %e\n", i, REAL(data,i),
                           IMAG(data,i));
                }
                int  iLoop = 0;
                //功率谱密度估计,获取各个脑电波频段的功率谱值   只需要取实部，因为虚部为0
                for( iLoop = 0; iLoop<COUNTN; iLoop++)
                {
                    //求功率谱，根据FFT计算出来的实部和虚部求模
                    m_DataPower.append((2.0/(Fs*COUNTN))*(fabs(REAL(data,iLoop))*fabs(REAL(data,iLoop)) +
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
                //saveEegWaveToEdfFile(m_Delta,m_Theta,m_Alpha,m_Beta,m_Gama, output_filename);
                gsl_fft_complex_wavetable_free (wavetable);
                gsl_fft_complex_workspace_free (workspace);

            }/*end if*/
        }/*end while*/

    }
}


