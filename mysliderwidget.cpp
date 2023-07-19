#include "mysliderwidget.h"
#include "ui_mysliderwidget.h"
#include <QCollator>

mySliderWidget::mySliderWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mySliderWidget)
    ,m_ptrLayoutMain(new QHBoxLayout(this))
{
    ui->setupUi(this);
    m_ptrStackWidget = new QStackedWidget;
    // ffemg库版本
    unsigned version = avcodec_version();
    qDebug()<< "verision" << version;

    this->initForm();
    loadMedia("../FFTW/sound_lib/mp4", "../FFTW/sound_lib/mp3");

}

void mySliderWidget::loadMedia(const QString& videoFilePath,const QString & audioFilePath)
{
    QDir dir(videoFilePath);
    QDir dirAudio(audioFilePath);

    QStringList filters;
    filters << "*.mp4" << "*.wav"<< "*.mp3";

    QFileInfoList VideofileInfoList = dir.entryInfoList(filters,QDir::Files,QDir::Name);   //按文件类型筛选，名字排序
    QFileInfoList AudiofileInfoList = dirAudio.entryInfoList(filters,QDir::Files,QDir::Name);

    if(VideofileInfoList.count()<=0 || AudiofileInfoList.count()<=0)
        return;
    QString outputFilePath = " ";
    dir.cdUp();
    QString DirPath = dir.path();
    //检查输出文件夹内是否为空
    QString dirfilePath = DirPath + "/output";
    QDir dirOut(dirfilePath);
    QFileInfoList outFileCout = dirOut.entryInfoList(QStringList()<<"*.mp4");
    QStringList outputFilePathList;
    int filecout = 0;
    if(outFileCout.count()== 0)
    {
        while (VideofileInfoList.count()> 0 && filecout <= VideofileInfoList.count()-1)
        {
            QString   videoFilePath = VideofileInfoList.at(filecout).absoluteFilePath();
            QString   audioFilePath = AudiofileInfoList.at(filecout).absoluteFilePath();
            qDebug() << videoFilePath;
            qDebug() << audioFilePath;
            QFileInfo videoFileInfo = QFileInfo(videoFilePath);
            QString   filename = videoFileInfo.fileName().split('.').at(0); // 文件名
            m_videoCombinaAudio.insert(videoFilePath,audioFilePath);
            QString   outputFilePath = QString(DirPath + "/output/%1_video.mp4").arg(filename);
            mergeVideoWithAudio(videoFilePath, audioFilePath, outputFilePath);
            outputFilePathList << outputFilePath;
            filecout ++;
        }
    }
    else
    {   //直接遍历文件夹
        for(int i=0;i< outFileCout.count();i++)
        {
            QString outputFilePath = outFileCout.at(i).absoluteFilePath();
            outputFilePathList<< outputFilePath;
        }
    }
    for(int i = 0; i < outputFilePathList.count(); ++i)
    {
        outputFilePath = outputFilePathList.at(i);
        QUrl url = QUrl::fromLocalFile(outputFilePath);
        playlist->addMedia(url);
        QFileInfo fileInfo(outputFilePath);
        ui->listWidget->addItem(fileInfo.fileName());
    }
    if (mVideoPlayer->state()!=QMediaPlayer::PlayingState)
        playlist->setCurrentIndex(0);
}


void mySliderWidget::mergeVideoWithAudio(const QString &videoFilePath, const QString &audioFilePath, const QString &outputFilePath)
{
    QString ffmpegCommand = "ffmpeg -i " + videoFilePath + " -i " + audioFilePath + " -c:v copy -c:a copy -map 0:v:0 -map 1:a:0 " + outputFilePath;
    QProcess process;
    process.start(ffmpegCommand);
    process.waitForFinished(-1); // 等待进程完成
}

void mySliderWidget::initForm()
{
    //初始化暂停键
    ui->btnPlay->setIcon(QPixmap(":/images/play.png"));
    this->setStyleSheet("background-color: rgb(147, 147, 147);");
    //按钮背景透明圆形有图案
    FlatUI::Instance()->setSliderQss(ui->sliderPosition);
    FlatUI::Instance()->setSliderQss(ui->sliderVolumn);
    for (int var = 0; var < m_videoNums; ++var)
    {
        m_videoWidegt = new MyVideoWidget;
        m_videoWidegt->setProperty("id",QVariant::fromValue(var));
        m_indexToWidget.insert(var,m_videoWidegt);
        m_ptrStackWidget->addWidget(m_videoWidegt);
    }
    m_windowWidth = m_ptrStackWidget->widget(m_currentIndex)->width();
    m_windowHieght = m_ptrStackWidget->widget(m_currentIndex)->height();
    m_ptrStackWidget->installEventFilter(this);
    m_ptrStackWidget->setMouseTracking(true);
    m_ptrStackWidget->setCurrentIndex(m_currentIndex);
    m_ptrStackWidget->setFixedSize(500, 500);
    m_ptrLayoutMain->addWidget(m_ptrStackWidget);//布局中加控件
    ui->groupBox->setLayout(m_ptrLayoutMain);//将布局添加进QGroupBox中。
    ui->btnSound->setStyleSheet("border-image: url(:/images/volumn.bmp);");

    //播放器
    mVideoPlayer = new QMediaPlayer(this);//创建视频播放器
    playlist = new QMediaPlaylist(this);
    mVideoPlayer->setNotifyInterval(2000);//信息更新周期
    playMode = QMediaPlaylist::Loop;
    m_videoWidegt = qobject_cast<MyVideoWidget*>(m_indexToWidget.first());//当前组件
    mVideoPlayer->setVideoOutput(m_videoWidegt);//视频显示组件
    m_videoWidegt->setMediaPlayer(mVideoPlayer);//设置显示组件的关联播放器
    playlist->setPlaybackMode(playMode);//默认歌曲列表循环
    mVideoPlayer->setPlaylist(playlist);
    connect(mVideoPlayer,SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SLOT(onstateChanged(QMediaPlayer::State)));

    connect(mVideoPlayer,SIGNAL(positionChanged(qint64)),
            this, SLOT(onPositionChanged(qint64)));

    connect(mVideoPlayer,SIGNAL(durationChanged(qint64)),
            this, SLOT(onDurationChanged(qint64)));

    connect(playlist,SIGNAL(currentIndexChanged(int)),
            this, SLOT(onPlaylistChanged(int)));

    connect(ui->listWidget,&QListWidget::currentRowChanged,this,[=](int val)
            {
                playlist->setCurrentIndex(val);
                if(mVideoPlayer->state() == QMediaPlayer::State::StoppedState)
                {
                    mVideoPlayer->play();
                }
            });


//    mVideoPlayer->stop();
}

mySliderWidget*mySliderWidget::m_instance = nullptr;
mySliderWidget *mySliderWidget::GetKernel()
{
    if(m_instance==nullptr)
    {
        m_instance= new mySliderWidget();
    }
    return m_instance;
}

mySliderWidget::~mySliderWidget()
{
    delete ui;
}

void mySliderWidget::onAnimationFinished()
{
    QParallelAnimationGroup* group = (QParallelAnimationGroup*)sender();
    QWidget* widget = group->property("widget").value<QWidget*>();
    if (nullptr != widget) {
        widget->hide();
    }
    m_bDonghua = false;
}

void mySliderWidget::onstateChanged(QMediaPlayer::State _state)
{
    //播放器状态变化，更新按钮状态
    //切换按钮的icon
    state = _state;
    ui->btnPlay->setEnabled(true);//保持使能状态
    if(state==QMediaPlayer::PlayingState)
        ui->btnPlay->setIcon(QPixmap(":/images/pause.png"));

    else if(state==QMediaPlayer::PausedState)
        ui->btnPlay->setIcon(QPixmap(":/images/play.png"));
}


void mySliderWidget::onDurationChanged(qint64 duration)
{
    //文件时长变化
    ui->sliderPosition->setMaximum(duration);
    int   secs=duration/1000;//秒
    int   mins=secs/60; //分钟
    secs=secs % 60;//余数秒
    durationTime = QString::asprintf("%d:%d",mins,secs);
    ui->LabRatio->setText(positionTime+"/"+durationTime);
}

void mySliderWidget::onPositionChanged(qint64 position)
{
    //文件播放位置变化
    if (ui->sliderPosition->isSliderDown())
        return; //如果正在拖动滑条，退出
    ui->sliderPosition->setSliderPosition(position);//
    int   secs=position/1000;//秒
    int   mins=secs/60; //分钟
    secs=secs % 60;//余数秒
    positionTime = QString::asprintf("%d:%d",mins,secs);
    ui->LabRatio->setText(positionTime+"/"+durationTime);
}
//qt会自动调用
bool mySliderWidget::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type()==QEvent::MouseButtonPress)
    {
        mousePressed=true;
        mousePos = static_cast<QMouseEvent*>(event)->pos();
        return true;// 返回true表示事件已经被处理
    }
    else if(event->type()==QEvent::MouseMove)
    {
        if(mousePressed){
            QPoint delta = static_cast<QMouseEvent*>(event)->pos() - mousePos;
            if(delta.x()>10)
            {// 向右滑动切换到下一个视频
                m_flagPreOrNext = 1;
                switchToNextVideo();
                animateVideoLabel();
            }
            else if(delta.x()<-10){
            // 向左滑动切换到上一个视频
                m_flagPreOrNext = 0;
                switchToPreviousVideo();
                animateVideoLabel();
            }
            return true;
        }
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        mousePressed = false;
        return true;
    }
    return false;
}

void mySliderWidget::switchToNextVideo()
{
    (m_currentVideoIndex)++;
    if (m_currentVideoIndex >= m_fileList.size())
    {
        m_currentVideoIndex = 0;
    }
    switchToVideo(m_currentVideoIndex);
}

void mySliderWidget::switchToPreviousVideo()
{
    (m_currentVideoIndex)--;
    if (m_currentVideoIndex < 0)
    {
        m_currentVideoIndex = m_fileList.size() - 1;
    }
    switchToVideo(m_currentVideoIndex);
}

void mySliderWidget::switchToVideo(int index)
{
    if(m_fileList.count()<1)
        return;
    QString videoFile= m_fileList[index];
    mVideoPlayer->setMedia(QUrl::fromLocalFile(videoFile));
    mVideoPlayer->play();
}

void mySliderWidget::animateVideoLabel()
{
    if(m_flagPreOrNext == 0)
        onSwitcnPreSong();
    else
        onSwitcnNextSong();
}

void mySliderWidget::on_btnPlay_clicked()
{
    int index = playlist->currentIndex();
    if(index>=0)
    {
      playlist->setCurrentIndex(index);
       if(state==QMediaPlayer::State::PlayingState)
            mVideoPlayer->pause();
       else if(state==QMediaPlayer::State::PausedState)
            mVideoPlayer->play();
    }
    else if(index<0)
        QMessageBox::information(this,"提示","请添加歌曲",QMessageBox::Button::Ok|QMessageBox::Button::Ignore);
}

void mySliderWidget::on_btnSound_clicked()
{
   //静音按钮
   bool mute = mVideoPlayer->isMuted();
   mVideoPlayer->setMuted(!mute);
   if (mute)
        ui->btnSound->setIcon(QIcon(":/images/images/volumn.bmp"));
   else
        ui->btnSound->setIcon(QIcon(":/images/images/mute.bmp"));
}

void mySliderWidget::on_sliderVolumn_valueChanged(int value)
{
   mVideoPlayer->setVolume(value);
}

void mySliderWidget::on_sliderPosition_valueChanged(int value)
{
   mVideoPlayer->setPosition(value);
}

void mySliderWidget::onSwitcnPreSong()
{
   int currentIndex = m_ptrStackWidget->currentIndex();
   if(0 == currentIndex){
        return;
   }
   if (m_bDonghua) {
        return;
   }
   //显示组件的切换
   int PreIndex = currentIndex - 1;
   m_videoWidegt = qobject_cast<MyVideoWidget*>(m_indexToWidget[PreIndex]);//前一个组件
   mVideoPlayer->setVideoOutput(m_videoWidegt);//视频显示组件
   m_videoWidegt->setMediaPlayer(mVideoPlayer);//设置显示组件的关联播放器
   playlist->setCurrentIndex(PreIndex);
   qDebug()<< QString("current id:")<<m_videoWidegt->property("id").toString();
   //动画滚动
   m_bDonghua = true;
   int windowWidth = m_ptrStackWidget->widget(currentIndex)->width();
   int windowHieght = m_ptrStackWidget->widget(currentIndex)->height();
//   int PreIndex = currentIndex - 1;
   m_ptrStackWidget->setCurrentIndex(PreIndex);
   m_ptrStackWidget->widget(currentIndex)->show();

   QPropertyAnimation* animation1;
   QPropertyAnimation* animation2;
   // 设置动画组
   QParallelAnimationGroup* group = new QParallelAnimationGroup;
   //设置当前页（未切换前）面页面的动画效果
   animation1 = new QPropertyAnimation(m_indexToWidget[currentIndex],
                                       "geometry");
   animation1->setDuration(500);
   animation1->setStartValue(QRect(0, 0, windowWidth, windowHieght));
   animation1->setEndValue(QRect(windowWidth, 0, windowWidth, windowHieght));

   //设置即将切换到界面的动画效果
   animation2 = new QPropertyAnimation(m_indexToWidget[PreIndex], "geometry");
   animation2->setDuration(500);
   animation2->setStartValue(QRect(-windowWidth, 0, windowWidth, windowHieght));
   animation2->setEndValue(QRect(0, 0, windowWidth, windowHieght));

   group->addAnimation(animation1);
   group->addAnimation(animation2);
   group->start();
   group->setProperty(
       "widget", QVariant::fromValue(m_indexToWidget[currentIndex]));
   connect(group, SIGNAL(finished()), this, SLOT(onAnimationFinished()));

   QListWidgetItem*item = ui->listWidget->currentItem();
   if(item)
        ui->LabCurMedia->setText(item->text());
}

void mySliderWidget::onSwitcnNextSong()
{
   int currentIndex = m_ptrStackWidget->currentIndex();
   int total_page = m_ptrStackWidget->count();
   if (currentIndex >= total_page - 1) {
        return;
   }
   if (m_bDonghua) {
        return;
   }
   m_bDonghua = true;
   int windowWidth = m_ptrStackWidget->widget(currentIndex)->width();
   int windowHieght = m_ptrStackWidget->widget(currentIndex)->height();
   int NextIndex = currentIndex + 1;
   if(NextIndex<= m_videoNums)
   {
       //显示组件切换
       m_videoWidegt = qobject_cast<MyVideoWidget*>(m_indexToWidget[NextIndex]);//下一个组件
       mVideoPlayer->setVideoOutput(m_videoWidegt);//视频显示组件
       m_videoWidegt->setMediaPlayer(mVideoPlayer);//设置显示组件的关联播放器
       playlist->setCurrentIndex(NextIndex);
       qDebug()<< QString("current id:")<< m_videoWidegt->property("id").toString();
       //组件切换了
       m_ptrStackWidget->setCurrentIndex(NextIndex);
       m_ptrStackWidget->widget(currentIndex)->show();

       QPropertyAnimation* animation1;
       QPropertyAnimation* animation2;
       QParallelAnimationGroup* group = new QParallelAnimationGroup;
       animation1 = new QPropertyAnimation(m_indexToWidget[currentIndex],
                                           "geometry");
       animation1->setDuration(500);
       animation1->setStartValue(QRect(0, 0, windowWidth, windowHieght));
       animation1->setEndValue(QRect(-windowWidth, 0, windowWidth, windowHieght));

       animation2 =
           new QPropertyAnimation(m_indexToWidget[NextIndex], "geometry");
       animation2->setDuration(500);
       animation2->setStartValue(QRect(windowWidth, 0, windowWidth, windowHieght));
       animation2->setEndValue(QRect(0, 0, windowWidth, windowHieght));

       group->addAnimation(animation1);
       group->addAnimation(animation2);
       group->start();
       group->setProperty(
           "widget", QVariant::fromValue(m_indexToWidget[currentIndex]));
       connect(group, SIGNAL(finished()), this, SLOT(onAnimationFinished()));

       QListWidgetItem*item = ui->listWidget->currentItem();
       if(item)
            ui->LabCurMedia->setText(item->text());
   }
}

void mySliderWidget::on_btnPreviousSong_clicked()
{
    playlist->previous();

}

void mySliderWidget::on_btnNextSong_clicked()
{
    playlist->next();
}

