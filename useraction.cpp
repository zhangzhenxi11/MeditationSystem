#include "useraction.h"
#include "ui_useraction.h"

UserAction::UserAction(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserAction)
{
    ui->setupUi(this);

    QString bandpass_filteringPath =
        "\\relax\\MycleanOutData.txt"; //bandpass滤波后保存路径

    QString stopPass_filteringPath = "\\relax\\MystoppassOutData.txt";

    QString Matlab_filteringPath =  "\\relax\\MaltbcleanData.txt";

    QString Power_spectrumPath = "\\relax\\relax_close_eyes_1_fyf_out.txt"; //功率谱数据保存路径

    QString sourcefileName = ":/file/relax/rawData.txt";  //选择路径

    QString sourcefileName1 = "E:/Code/FFTW/relax/attention_read_1_fyf.txt";

    /*
     * *****************************bandpass   start*****************************
     * vectord cof_a,cof_b;
     * coeffBandPass(5, 1, 100, Fs, cof_a, cof_b);// coefficients of b,a
       vectord inputdata;
       vectord  outputdata;
       for (auto it = savearray.begin(); it!= savearray.end();it++)
       {
           inputdata.push_back(*it);
       }
       Myfilter fiter_test;
       fiter_test.filtfilt(cof_b, cof_a, inputdata, outputdata);

       for (auto it = outputdata.begin(); it!=outputdata.end();it++)
       {
           qDebug()<<*it;
       }

       for(auto &it:outputdata)
       {
           m_filt_all_eeg.push_back(it);//尾部添加
       }

       saveDataToFile(bandpass_filteringPath,m_filt_all_eeg);
       //3.读取处理过的数据，绘制曲线
       mlineNames.clear();
       mlineNames<<"cleanLine";
       drawDynamic(ui->customplot,1,mlineNames);
    *****************************bandpass   end*****************************
*/
}

UserAction::~UserAction()
{
    delete ui;
}

void UserAction::drawDynamic(QCustomPlot *plot1, int plot_num, QList<QString> _idxList)
{
    plot = plot1;
    idxList = _idxList;//备份，可能别的函数需要它
    //设置基本坐标轴（左侧Y轴和下方X轴）可拖动、可缩放、曲线可选、legend可选、设置伸缩比例，使所有图例可见
    plot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom
                          |QCP::iSelectAxes|QCP::iSelectLegend|QCP::iSelectPlottables);
    //坐标轴
    for(int i = 0;i< plot_num;i++)
    {
        plot->addGraph();
        plot->graph(i)->setName(idxList.at(i));
    }
    //遍历颜色枚举
    Qt::GlobalColor color_;
    for(color_ = Qt::GlobalColor::red; color_<= Qt::GlobalColor::darkRed; color_=(Qt::GlobalColor)(color_+1))
    {
        QPen pen;
        pen.setColor(QColor(color_));
        plot->graph()->setPen(pen);       // 设置画笔
        plot->legend->setVisible(true); // 显示图例
        plot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop|Qt::AlignLeft);//设置图例的位置
    }
    QSharedPointer<QCPAxisTickerTime>timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    plot->xAxis->setTicker(timeTicker);
    plot->axisRect()->setupFullAxesBox();
    // make left and bottom axes transfer their ranges to right and top axes:
    connect(plot->xAxis,SIGNAL(rangeChanged(QCPRange)),
            plot->xAxis2,SLOT(setRange(QCPRange)));

    connect(plot->yAxis,SIGNAL(rangeChanged(QCPRange)),
            plot->yAxis2,SLOT(setRange(QCPRange)));

    //设置选框,先开启选框
    plot->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);//模式：框选放大
    plot->selectionRect()->setPen(QPen(Qt::black,1,Qt::DashLine));//设置选框的样式：虚线
    plot->selectionRect()->setBrush(QBrush(QColor(0,0,100,50)));//设置选框的样式：半透明浅蓝
    plot->setSelectionRectMode(QCP::SelectionRectMode::srmZoom);
    // 框选放大和拖动无法同时使用
    plot->setInteraction(QCP::iRangeDrag,true);//使能拖动
    //开启子线程 ，定时器移动到子线程
    dataTimer=new QTimer();//定时器
    connect(dataTimer,SIGNAL(timeout()),this,SLOT(realTimeDataSlot()));
    dataTimer->start(0);//间隔0表示尽可能快地刷新
}

double UserAction::iterationData(QVector<double> &itContainer)
{
    static double first = 0;//初始量
    for( auto itor = itContainer.begin(); itor != itContainer.end(); itor++ )
    {
        if(*itor!=first)
        {
            qDebug()<<"GamaVect of size:"<< itContainer.size();
            itor= itContainer.erase(itor);
            first = *itor;
            qDebug()<<" *itor:"<< *itor<<"first:"<< first;
            break;
        }
    }
    return first;
}

void UserAction::readColumnData(const QString &filename)
{
    using namespace std;
    {
        char filepath[1024] = { 0 };
        GetModuleFileNameA(NULL, filepath, MAX_PATH);		// 获取到完整路径，
        *strrchr(filepath, '\\') = '\0';					// 截取路径
        cout <<filepath<<endl;
        ifstream ifs;                                       //创建流对象
        string path = string(filepath)+ filename.toStdString();
        ifs.open(path, ios::in);	//打开文件
        if (!ifs.is_open())						//判断文件是否打开
        {
            cout << "打开文件失败！！！";
        }
        //读取正确匹配特征点
        vector<string> item;				//用于存放文件中的一行数据
        string temp;						//把文件中的一行数据作为字符串放入容器中
        while (getline(ifs, temp))          //利用getline（）读取每一行，并放入到 item 中
        {
            item.push_back(temp);
        }
        for (auto it = item.begin(); it != item.end(); it++)
        {
            istringstream istr(*it);                 //其作用是把字符串分解为单词(在此处就是把一行数据分为单个数据)
            string str;
            int count = 0;							 //统计一行数据中单个数据个数
            //获取文件中的第 0、1、2、3、4 列数据
            while (istr >> str)                      //以空格为界，把istringstream中数据取出放入到依次str中
            {
                if (count == 0)
                {
                    double data_r = stod(str);
                    DeltaVect.push_back(data_r);
                }
                //获取第1列数据
                else if (count == 1)
                {
                    double data_r = stod(str);      //数据类型转换，将string类型转换成float,如果字符串不是有数字组成，则字符被转化为 0

                    ThetaVect.push_back(data_r);
                }
                //获取第2列数据
                else if (count == 2)
                {
                    double data_r = stod(str);       //数据类型转换，将string类型转换成float

                    AlphaVect.push_back(data_r);
                }
                else if(count == 3)
                {
                    double data_r =  stod(str);
                    BetaVect.push_back(data_r);
                }
                else if(count == 4)
                {
                    double data_r = stod(str);
                    GamaVect.push_back(data_r);
                }
                else if(count==6)
                {
                    double data_r = stod(str);
                    OtherVect.push_back(data_r);
                }
                count++;

            }
        }

    }



}

void UserAction::saveEegWaveToOutputFile(double &delta, double &theta, double &alpha, double &beta, double &gama, QString &filename)
{
    double total                      = (delta + theta + alpha + beta + gama );
    double delta_DIVIDE_total         = delta/total;
    double theta_DIVIDE_total         = theta/total;
    double alpha_DIVIDE_total         = alpha/total;
    double beta_DIVIDE_total          = beta/total;
    double gama_DIVIDE_total          = gama/total;

    double alpha_DIVIDE_beta                = alpha/beta ;
    double alpha_theta_DIVIDE_beta          = (alpha+theta)/beta;
    double alpha_theta_DIVIDE_alpha_beta    = (alpha+theta)/(alpha+beta) ;
    double theta_DIVIDE_beta                = theta/beta ;

    double attention_Yao                    = 1.0 - (alpha)/(alpha + theta) ;
    double attention_Zhang618               = (alpha + beta)/(theta + alpha + beta);
    double attention_ZhangR                 = (alpha + beta + gama) / (theta + alpha + beta + gama);
    double attention_Su                     = (beta)/(alpha+theta);
    double attention_Su_Yuan                = (beta)/(alpha+theta+beta);


    int buff_number = snprintf(m_buf,650,"%-30.7f  %-30.7f  %-30.7f  %-30.7f  %-30.7f  "
                                           "%-30.7f  %-30.7f  %-30.7f  %-30.7f  %-30.7f  "
                                           "%-30.7f  %-30.7f  %-30.7f  %-30.7f  "
                                           "%-30.7f  %-30.7f  %-30.7f  %-30.7f  %-30.7f  \n",
                               delta,
                               theta,
                               alpha,
                               beta,
                               gama,

                               delta_DIVIDE_total,
                               theta_DIVIDE_total,
                               alpha_DIVIDE_total,
                               beta_DIVIDE_total,
                               gama_DIVIDE_total,

                               alpha_DIVIDE_beta,
                               alpha_theta_DIVIDE_beta,
                               alpha_theta_DIVIDE_alpha_beta,
                               theta_DIVIDE_beta,

                               attention_Yao,
                               attention_Zhang618,
                               attention_ZhangR,
                               attention_Su,
                               attention_Su_Yuan
                               );
    QFile file(QApplication::applicationDirPath()+filename);
    bool ret = file.open(QIODevice::Append | QIODevice::Text);//字符串
    if(ret)
        file.write(m_buf);
    else
        qDebug()<<QString::fromLocal8Bit("%1:device not open!").arg(filename) << file.errorString();
    file.close();

}

void UserAction::eegDataProcess(QString &input_filename, QString &output_filename)
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
        QTextStream stream(&file);
        while(!file.atEnd())
        {
            QString str =stream.readLine();
            m_EegDataBuf = str.toDouble();
            m_array.append(m_EegDataBuf);
            if(m_array.size()== N)
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
                    REAL(data,i)= m_array[i];//z[2i]        z[0]  z[2]
                    IMAG(data,i) = 0.0;      //z[2i+1]      z[1]  z[3]
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
                           IMAG(data,i));//例如：505: 8.565403e-01 -4.848547e-17
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
                    m_Delta += m_DataPower[iLoop];
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
                m_array.clear();//清除缓存,才开启下个循环
                //保存到文件中
                saveEegWaveToOutputFile(m_Delta,m_Theta,m_Alpha,m_Beta,m_Gama, output_filename);

            }/*end if*/
        }/*end while*/
        gsl_fft_complex_wavetable_free (wavetable);
        gsl_fft_complex_workspace_free (workspace);
    }
}

void UserAction::saveDataToFile(QString &filename, QVector<double> clean_array)
{
    qDebug() << QApplication::applicationDirPath()+filename;
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

void UserAction::ReadDataToContainer(QString &filename)
{
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QTextStream stream(&file);
        while (!stream.atEnd())
        {
            double buf;
            stream>>buf;
            savearray.append(buf);
        }
        file.close();
    }
}

void UserAction::GaussianFilter()
{
    const size_t K = 51;                         /* window size */
    const double alpha[3] = { 0.5, 3.0, 10.0 };  /* alpha values */

    gsl_vector *x = gsl_vector_alloc(N);         /* input vector */
    gsl_vector *y1 = gsl_vector_alloc(N);        /* filtered output vector for alpha1 */
    gsl_vector *y2 = gsl_vector_alloc(N);        /* filtered output vector for alpha2 */
    gsl_vector *y3 = gsl_vector_alloc(N);        /* filtered output vector for alpha3 */

    gsl_vector *k1 = gsl_vector_alloc(K);        /* Gaussian kernel for alpha1 */
    gsl_vector *k2 = gsl_vector_alloc(K);        /* Gaussian kernel for alpha2 */
    gsl_vector *k3 = gsl_vector_alloc(K);        /* Gaussian kernel for alpha3 */

    gsl_rng *r = gsl_rng_alloc(gsl_rng_default);//默认种子gsl_rng_default_seed自动初始化生成器

    gsl_filter_gaussian_workspace *gauss_p = gsl_filter_gaussian_alloc(K);
    size_t i;
    double sum = 0.0;

    /* generate input signal */
    for (i = 0; i < N; ++i)
    {
        double ui = gsl_ran_gaussian(r, 1.0);
        sum += ui;
        gsl_vector_set(x, i, sum);
    }

    /* compute kernels without normalization */
    gsl_filter_gaussian_kernel(alpha[0], 0, 0, k1);
    gsl_filter_gaussian_kernel(alpha[1], 0, 0, k2);
    gsl_filter_gaussian_kernel(alpha[2], 0, 0, k3);

    /* apply filters */
    gsl_filter_gaussian(GSL_FILTER_END_PADVALUE, alpha[0], 0, x, y1, gauss_p);
    gsl_filter_gaussian(GSL_FILTER_END_PADVALUE, alpha[1], 0, x, y2, gauss_p);
    gsl_filter_gaussian(GSL_FILTER_END_PADVALUE, alpha[2], 0, x, y3, gauss_p);

    /* print kernels */
    for (i = 0; i < K; ++i)
    {
        double k1i = gsl_vector_get(k1, i);
        double k2i = gsl_vector_get(k2, i);
        double k3i = gsl_vector_get(k3, i);

        printf("%e %e %e\n", k1i, k2i, k3i);
    }

    printf("\n\n");

    /* print filter results */
    for (i = 0; i < N; ++i)
    {
        double xi = gsl_vector_get(x, i);
        double y1i = gsl_vector_get(y1, i);
        double y2i = gsl_vector_get(y2, i);
        double y3i = gsl_vector_get(y3, i);
        gaussian_x.append(xi);
        gaussian_y1.append(y1i);
        gaussian_y2.append(y2i);
        gaussian_y3.append(y3i);
        printf("%.12e %.12e %.12e %.12e\n", xi, y1i, y2i, y3i);
    }
    gsl_vector_free(x);
    gsl_vector_free(y1);
    gsl_vector_free(y2);
    gsl_vector_free(y3);
    gsl_vector_free(k1);
    gsl_vector_free(k2);
    gsl_vector_free(k3);
    gsl_rng_free(r);
    gsl_filter_gaussian_free(gauss_p);
}

void UserAction::coeffBandPass(int n, double lowcut, double highcut, int fs, vectord &acof_vec, vectord &bcof_vec)
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

void UserAction::coffStopPass(int n, double lowcut, double highcut, int fs, vectord &ccof_vec, vectord &dcof_vec)
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

void UserAction::realTimeDataSlot()
{
    static QTime timeStart = QTime::currentTime();
    double key_x = timeStart.msecsTo(QTime::currentTime())/1000.0; // time elapsed since start of demo, in seconds
    static double lastPointKey = 0;
    if(flag_start==1)
    {
        if (key_x-lastPointKey > 0.2) // at most add point every 200 ms
        {
            /* add data to lines: */
            /*
            double soruceiganl =iterationData(savearray);
            double bandsugnal = iterationData(m_filt_all_eeg);
            */
            if(plot==ui->customplot_2)
            {
                plot->graph(0)->addData(key_x, m_value);
            }
            plot->graph(0)->addData(key_x, m_value);
            lastPointKey = key_x;
        }
        //使坐标轴可以伸缩
        QList<QCPAxis*> axes;
        axes << plot->yAxis2 << plot->xAxis2 << plot->yAxis << plot->xAxis;
        plot->axisRect()->setRangeZoomAxes(axes);

        //图像最高点最低点自动缩放坐标轴 y轴
        for (int i= 0; i< mlineNames.size();++i)
        {
            plot->graph(i)->rescaleValueAxis();
        }
        plot->yAxis->rescale(true);//y轴值自动变化
        plot->xAxis->setRange(key_x,8,Qt::AlignRight);
        plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
        plot->replot();
    }

}

void UserAction::deletePoiter()
{
    if(plot!=nullptr)
        delete plot;
    plot= nullptr;
}

void UserAction::on_btnStart_clicked()
{
    if (flag_start==0)
    {
        ui->btnStart->setEnabled(false);
        ui->btnStop->setEnabled(true);
    }
    flag_start =1;
}

void UserAction::on_btnStop_clicked()
{
    if (flag_start==1)
    {
        ui->btnStart->setEnabled(true);
        ui->btnStop->setEnabled(false);
    }
    flag_start = 0;
}

void UserAction::newDataAvailable(int value)
{
    m_value = value;
}
