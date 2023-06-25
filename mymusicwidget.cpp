#include "mymusicwidget.h"
#include "ui_mymusicwidget.h"
#include <QFileDialog>
#include <QMatrix>



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
    loadMedia("../FFTW/sound_lib/music");
    //分支功能测试 dev
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

void MyMusicWidget::initForm()
{
    //声音滑动条
    FlatUI::Instance()->setSliderQss(ui->Volumn);
    FlatUI::Instance()->setSliderQss(ui->Position);
    //初始化播放器模式
    ui->btnMode->setStyleSheet("border-image: url(:/images/xunhuan.png);");
    //初始化暂停键
    ui->btnPlay->setIcon(QPixmap(":/images/620.bmp"));
    this->setStyleSheet("background-color: rgb(147, 147, 147);");
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
    ui->Position->setMaximum(duration);
    int   secs=duration/1000;//秒
    int   mins=secs/60; //分钟
    secs=secs % 60;//余数秒
    durationTime=QString::asprintf("%d:%d",mins,secs);
    ui->LabRatio->setText(positionTime+"/"+durationTime);
}

void MyMusicWidget::onPositionChanged(qint64 position)
{
    //当前文件播放位置变化，更新进度显示
    if (ui->Position->isSliderDown())
        return;
    ui->Position->setSliderPosition(position);//
    qInfo()<< position;
    int   secs=position/1000;//秒
    int   mins=secs/60; //分钟
    secs=secs % 60;//余数秒
    positionTime=QString::asprintf("%d:%d",mins,secs);
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
    ui->btnMode->style()->unpolish(ui->btnMode);
    ui->btnMode->style()->polish(ui->btnMode);
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
    //手动更新样式
    ui->btnMode->style()->unpolish(ui->btnMode);	//清理之前的样式
    ui->btnMode->style()->polish(ui->btnMode);		//用于添加新的样式
    ui->btnMode->update();
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

MyMusicWidget*MyMusicWidget::Instance = nullptr;


