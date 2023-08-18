#include "mymusicwidget.h"
#include "ui_mymusicwidget.h"
#include <QFileDialog>
#include <QMenu>

MyMusicWidget::MyMusicWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyMusicWidget),
    player(new QMediaPlayer(this)),
    playlist(new QMediaPlaylist(this))
{
    ui->setupUi(this);
    this->initForm();
    player->setPlaylist(playlist);

    connect(player,SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SLOT(onStateChanged(QMediaPlayer::State)));

    connect(player,SIGNAL(positionChanged(qint64)),
            this, SLOT(onPositionChanged(qint64)));

    connect(player,SIGNAL(durationChanged(qint64)),
            this, SLOT(onDurationChanged(qint64)));

    connect(playlist,SIGNAL(currentIndexChanged(int)),
            this, SLOT(onPlaylistChanged(int)));

    connect(ui->listWidget,&QListWidget::currentRowChanged,this,[=](int val)
            {
                playlist->setCurrentIndex(val);
                if(player->state() == QMediaPlayer::State::StoppedState)
                {
                    player->play();
                    state = QMediaPlayer::State::PlayingState;
                    timer->start();
                }
            });
    dataProcessing = MyDataProcessing::GetKernel();
    //连接关注度槽函数  dataProcessing在子线程中,接收者在主线程
   /*
    QtConcurrent::run()函数的参数如下：
        第一个参数是要执行的函数，可以是全局函数、静态成员函数或Lambda表达式。
        后续的参数是要传递给执行函数的参数。
    */
    synTimer = new QTimer();
    connect(synTimer,&QTimer::timeout,this,[=](){
//        connect(dataProcessing,SIGNAL(dynamicAttentionData(double)),this,SLOT(reciveAttention(double)),Qt::QueuedConnection);
    //    connect(dataProcessing,&MyDataProcessing::dynamicAttentionData,this,[=](){
    //    QtConcurrent::run(std::bind(&MyMusicWidget::reciveAttention, MyMusicWidget::GetKernal()));
    //    QtConcurrent::run(std::mem_fn(&MyMusicWidget::reciveAttention),MyMusicWidget::GetKernal());
    //    });
    });

    synTimer->start(5000);
    loadMedia("../FFTW/sound_lib/music");
//    playMode = QMediaPlaylist::Loop;
//    playlist->setPlaybackMode(playMode);
}

MyMusicWidget *MyMusicWidget::GetKernal()
{
    if(Instance==nullptr)
    {
      Instance =new MyMusicWidget;
    }
    return Instance;
}

MyMusicWidget::~MyMusicWidget()
{
    delete ui;
}

void MyMusicWidget::initAction()
{
    action5->setIcon(QIcon(":/images/pf.png"));
    action5->setText(("设置皮肤"));
    action5_1->setIcon(QIcon(":/images/pf2.png"));
    action5_1->setText(("绿色"));
    action5_2->setIcon(QIcon(":/images/pf5.png"));
    action5_2->setText(("动漫"));
    action5_4->setIcon(QIcon(":/images/pf.png"));
    action5_4->setText(("蓝色"));
    action5_3->setIcon(QIcon(":/images/pf4.png"));
    action5_3->setText(("自定义"));
    action5_3->setShortcut(QKeySequence("Ctrl+X"));
    QMenu * ChangeBackGroundMenu = new QMenu(this);//设置皮肤菜单
    ChangeBackGroundMenu->addAction(action5_1);
    ChangeBackGroundMenu->addAction(action5_2);
    ChangeBackGroundMenu->addAction(action5_4);
    ChangeBackGroundMenu->addAction(action5_3);
    ChangeBackGroundMenu->setStyleSheet(MenuStyle());
    action5->setMenu(ChangeBackGroundMenu);
    btn3->setMenu(ChangeBackGroundMenu);
}

QString MyMusicWidget::MenuStyle()
{
    return
        " QMenu {"
           " background: white; "
           "border-radius:2px;"
           "border:1px solid rgb(200,200,200);"

           "  }"
           "  QMenu::item {"

           " background-color: transparent;"
           " padding:5px 30px;"
           "       margin:0px 0px;"
           " border-bottom:0px solid #DBDBDB;"
           "}"
           " QMenu::item:selected { "
           "background-color: aqua;"
        "}";
}

QString MyMusicWidget::PlayStyle()
{

}

QString MyMusicWidget::PaseStyle()
{

}

QString MyMusicWidget::IlikeStyle()
{

}

QString MyMusicWidget::Ilike1Style()
{

}

QString MyMusicWidget::ShowListStyle()
{

}

QString MyMusicWidget::HideListStyle()
{

}

QString MyMusicWidget::RandomStyle()
{

}

QString MyMusicWidget::LoopStyle()
{

}

QString MyMusicWidget::LoopOneStyle()
{

}

QString MyMusicWidget::VoiceStyle()
{

}

QString MyMusicWidget::NoVoiceStyle()
{

}

QString MyMusicWidget::MusicListStyle()
{

}

void MyMusicWidget::initForm()
{
    //声音滑动条
//    FlatUI::Instance()->setSliderQss(ui->Volumn);
//    FlatUI::Instance()->setSliderQss(ui->Position);
    this->adjustSize();
    this->setFixedSize(width(),height());
    //初始化播放器模式
    ui->btnMode->setStyleSheet("border-image: url(:/images/xunhuan.png);");
    //初始化暂停键
    ui->btnPlay->setIcon(QPixmap(":/images/image/pase.png"));
    //整体页面背景
    //  this->setStyleSheet("background-color: rgb(147, 147, 147);");
    //唱片
    pix= new QPixmap(":/images/record.png");
    ui->lblRecord->setScaledContents(true);
    ui->lblRecord->setPixmap(*pix);
    //唱片指针图片
    picrneedle = new QPixmap(":/images/pipe0.png");
    ui->lblneedle->setPixmap(*picrneedle);
    transformPicutrue();//旋转槽函数
    recordTimer=  new QTimer(this);
    recordRotation =0;
    // 连接定时器的信号与槽函数
    connect(recordTimer,&QTimer::timeout,this,&MyMusicWidget::needleAnimationDynamic);
    recordTimer->start(100);
    // 设置唱片旋转的自定义Label的对齐方式和背景透明
    ui->lblneedle->setAlignment(ui->lblneedle->alignment());
    ui->lblneedle->setAttribute(Qt::WA_TranslucentBackground);
    //自定义动画按钮
    btn1 = new QPushButton(this);
    btn2 = new QPushButton(this);
    btn3 = new QPushButton(this);
    btnPull = new QPushButton(this);
    btnPull->setGeometry(width()-32,     0,32,32);
    btn3->setGeometry(btnPull->x()- 32, -32, 32, 32);
    btn2->setGeometry(btn3->x()- 32,    -32, 32, 32);
    btn1->setGeometry(btn2->x()- 32,    -32, 32, 32);

    auto anima1  =new QPropertyAnimation(btn1,"pos",this);
    auto anima2  =new QPropertyAnimation(btn2,"pos",this);
    auto anima3  =new QPropertyAnimation(btn3,"pos",this);

    anima1->setStartValue(QPoint(btn1->x(),-32));
    anima1->setEndValue(QPoint(btn1->x(),0));
    anima1->setDuration(500);
    anima1->setEasingCurve(QEasingCurve::Type::InOutBounce);

    anima2->setStartValue(QPoint(btn2->x(),-32));
    anima2->setEndValue(QPoint(btn2->x(),0));
    anima2->setDuration(500);
    anima2->setEasingCurve(QEasingCurve::Type::InOutBounce);

    anima3->setStartValue(QPoint(btn3->x(),-32));
    anima3->setEndValue(QPoint(btn3->x(),0));
    anima3->setDuration(500);
    anima3->setEasingCurve(QEasingCurve::Type::InOutBounce);

    auto group = new QSequentialAnimationGroup(this);
    group->addAnimation(anima1);
    group->addAnimation(anima2);
    group->addAnimation(anima3);
    connect(btnPull, &QPushButton::clicked, this, [=]()
            {
                if (group->state() == QAbstractAnimation::State::Stopped &&
                    group ->direction() == QAbstractAnimation::Direction::Forward &&
                    btn3->y()>=0)
                {
                    group->setDirection(QAbstractAnimation::Direction::Backward);
                }
                else
                {
                    group->setDirection(QAbstractAnimation::Direction::Forward);
                }

                group->start();
            });

}

void MyMusicWidget::loadMedia(const QString &filePath)
{
    QDir dir(filePath);
    QStringList filters;
    filters << "*.mp3" << "*.wav";
    QFileInfoList fileInfoList = dir.entryInfoList(QStringList()<<"*.mp3");
    if(fileInfoList.count()<=0)
      return;
    for (int i = 0; i < fileInfoList.count(); ++i)
    {
      QString filepath = fileInfoList.at(i).absoluteFilePath();
      QUrl url = QUrl::fromLocalFile(filepath);
      playlist->addMedia(url);
      QFileInfo fileInfo(filepath);
      ui->listWidget->addItem(fileInfo.fileName());
    }
}

void MyMusicWidget::transformPicutrue()
{
    // 创建 QTimer
    timer = new QTimer;
    // 每隔 10ms 就触发一次超时信号
    timer->setInterval(10);//每隔10ms,去执行下面的方法：旋转图片一度。
    // 记录当前旋转的角度
    int currentAngle = 0;
    connect(timer, &QTimer::timeout, [&](){
        // 每次旋转 1 度
        currentAngle = (currentAngle + 1) % 360;
        QTransform transform;
        // 将旋转的中心点设置为图片的中心
        transform.translate(pix->width()/2, pix->height()/2);
        transform.rotate(currentAngle);
        transform.translate(-pix->width()/2, -pix->height()/2);
        // 将变换应用到 QPixmap 上，并更新 QLabel
        ui->lblRecord->setPixmap(pix->transformed(transform, Qt::FastTransformation));

        if(state == QMediaPlayer::StoppedState)
        {
            m_bPlay = false;
            timer->stop();
        }
        else
        {
            m_bPlay = true;
            timer->start();
        }
    });

}

void MyMusicWidget::onStateChanged(QMediaPlayer::State _state)
{
    //播放器状态变化，更新按钮状态
    //切换按钮的icon
    state = _state;
    ui->btnPlay->setEnabled(true);//保持使能状态
    if(state == QMediaPlayer::PlayingState)
        ui->btnPlay->setIcon(QPixmap(":/images/622.bmp"));

    else if(state == QMediaPlayer::PausedState)
        ui->btnPlay->setIcon(QPixmap(":/images/620.bmp"));
}

void MyMusicWidget::onPlaylistChanged(int position)
{
    //播放列表变化,更新当前播放文件名显示
    ui->listWidget->setCurrentRow(position);
    QListWidgetItem  *item = ui->listWidget->currentItem();
    if (item)
        ui->LabCurMedia->setText(item->text());
}

void MyMusicWidget::onDurationChanged(qint64 duration)
{
    //文件时长变化，更新进度显示
    MusicDuration = duration;
    ui->horizontalSlider->setMaximum(MusicDuration);
    int secs = MusicDuration/1000;//秒
    int mins = secs/60; //分钟
    secs = secs % 60;//余数秒
    durationTime = QString::asprintf("%d:%d",mins,secs);
    ui->LabRatio->setText(positionTime+"/"+durationTime);
}

void MyMusicWidget::onPositionChanged(qint64 position)
{
    //当前文件播放位置变化，更新进度显示
    MusicPostion = position;
    if (ui->horizontalSlider->isSliderDown())
        return;
    ui->horizontalSlider->setSliderPosition(MusicPostion);//
    qInfo()<< MusicPostion;
    int secs = MusicPostion/1000;//秒
    int mins = secs/60; //分钟
    secs = secs % 60;//余数秒
    positionTime = QString::asprintf("%d:%d",mins,secs);
    ui->LabRatio->setText(positionTime+"/"+durationTime);
}

void MyMusicWidget::on_btnPrevious_clicked()
{
    //前一文件
    playlist->previous();
}

void MyMusicWidget::on_btnPlay_clicked()
{
    //默认播放第一行歌曲
    if(ui->listWidget->count()>0)
    {
        m_currentIndex = playlist->currentIndex();
        if (m_currentIndex>=0)
        {
            playlist->setCurrentIndex(m_currentIndex);
            if(state==QMediaPlayer::State::PlayingState)
            {
                player->pause();//暂停播放
                m_bPlay=false;
                timer->stop();
            }
            else if(state==QMediaPlayer::State::PausedState)
            {
                player->play();//开始播放
                m_bPlay=true;
                timer->start();
            }
        }
        else if(m_currentIndex<0)
            QMessageBox::information(this,"提示","请选中歌曲",QMessageBox::Button::Ok|QMessageBox::Button::Ignore);
    }
    else
        QMessageBox::information(this,"提示","请添加歌曲",QMessageBox::Button::Ok|QMessageBox::Button::Ignore);

}

void MyMusicWidget::on_btnNext_clicked()
{
    //下一文件
    playlist->next();
}

void MyMusicWidget::on_sliderPosition_valueChanged(int value)
{
    //文件进度调控
    player->setPosition(value);
}

void MyMusicWidget::on_sliderVolumn_valueChanged(int value)
{
    //调整音量
    player->setVolume(value);
}

void MyMusicWidget::on_btnMode_clicked()
{
    //改写成状态机
    if(m_IsMode%4==0)//循环
    {
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
        ui->btnMode->setStyleSheet("border-image: url(:/images/xunhuan.png);");
    }
    if(m_IsMode%4==1)//单曲
    {
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
        ui->btnMode->setStyleSheet("border-image: url(:/images/danqu.png);");
    }
    if(m_IsMode%4==2)//单曲循环
    {
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        ui->btnMode->setStyleSheet("border-image: url(:/images/danquxunhuan.png);");
    }
    if(m_IsMode%4==3)//随机
    {
        playlist->setPlaybackMode(QMediaPlaylist::Random);
        ui->btnMode->setStyleSheet("border-image: url(:/images/suiji.png);");
    }
    m_IsMode++;
}

void MyMusicWidget::needleAnimationDynamic()
{
    if(m_bPlay)//开始音乐
    {
        recordRotation=45;
    }
    else
    {
        recordRotation=0;
    }
    ui->lblneedle->setRotationAngle(recordRotation);

}

void  MyMusicWidget::reciveAttention(double attenDataValue)
{
    /*
     * 前提：
     * 1.此函数在开启子线程中。
     * 2.默认必须是顺序播放，不可随机模式。
    1. 把数值划分到[0-100]区间内方法，获取关注度的区间1-100,分区间[0-25],[25-50] [50-75],[75-100]
    2. 先让用户自己点一首音乐，在播放期间用缓冲区buffer缓存关注度值。
    3. 当前播放结束后,计算缓冲区平均值，调用数值缩放方法取整数，即是播放索引值，切换下一首音乐。
    4. 用互斥锁保护播放索引值,防止其他线程竞争资源。
    5. 当前播放的音乐结束，解锁，循环3~4步骤。
    6. 注意： 开启多线程，让繁杂操作子线程做，不卡住主线程。
    */

    //主线程中长期运行的循环调用QApplication::processEvents()，以使执行工作时图形用户界面可以保持响应
//    QApplication::processEvents(QEventLoop::AllEvents, 1000);
    if(state == QMediaPlayer::State::PlayingState)//首先播放一首音乐
    {
        qDebug()<< "attenDataValue:"<< attenDataValue << Qt::endl;
        //获取音乐列表行数
        int countNum = playlist->mediaCount();
        //数值缩放到音乐列表数区间[1-maxcount]
        double min_value = 1;
        double max_value = countNum;
        double scaled_value = (attenDataValue - min_value) / (max_value - min_value);
        double mapped_value = scaled_value;
        mCurrentBuffer.append(mapped_value);
        double sum = 0;
        m_averageValue = 0;
//        qint64 MusicDuration = 0;
//        qint64 MusicPostion = 0;

        //当前音乐播放结束后 如果存在与此媒体对象关联的元数据，则返回true，否则返回false
//        if (player->isMetaDataAvailable())
//        {
//            MusicDuration = player->duration()/1000;
//            MusicPostion = player->position()/1000;
//        }
        qDebug() << "Duration:" << MusicDuration << "seconds"<< Qt::endl<< "position: "  << MusicPostion << "seconds";
        //计算上一首音乐期间的缓冲区平均值
        if(MusicPostion == MusicDuration && (MusicDuration!=0)) /*当前播放进度值是否达到当前音乐末尾*/
            {//根据关注度值，播放下一首音乐
                mNextBuffer = mCurrentBuffer;
                mCurrentBuffer.clear();
                //计算容器内平均值
                for(auto it = mNextBuffer.begin();it!= mNextBuffer.end();it++)
                {
                    qDebug()<< *it<< Qt::endl;
                    sum = sum + (*it);
                }
                m_averageValue = sum/(mNextBuffer.count());
                if(m_averageValue>countNum)
                {
                    qDebug()<< "out of index";
                    return;
                }
                //上锁
//                m_mutex.lock();
                m_currentIndex = int(m_averageValue);//临界资源
                qDebug()<<QString("当前播放音乐的索引:")<< m_currentIndex;
                //播放音乐
                playlist->setCurrentIndex(m_currentIndex);
                player->play();
                if(state == QMediaPlayer::State::PlayingState)
                {
                    timer->start();
                };
                //获取当前歌曲进度值
//                if(MusicPostion == MusicDuration)
//                {
//                    //当前歌播放结束后，先解锁
//                    m_mutex.unlock();
//                }
                //清空缓存
                mNextBuffer.clear();
                //休眠1000毫秒
                QThread::msleep(1000);
                QCoreApplication::processEvents(QEventLoop::AllEvents,100);
                }
            }

}


MyMusicWidget*MyMusicWidget::Instance = nullptr;



void MyMusicWidget::on_btnMusic_clicked()
{

}


void MyMusicWidget::on_btnRepeat_clicked()
{//隐藏音乐列表
    ui->listWidget->setGeometry(30,120,321,481);
    auto animal = new QPropertyAnimation(ui->listWidget,"pos",this);
//    animal->setStartValue();



}

