#ifndef MYMUSICWIDGET_H
#define MYMUSICWIDGET_H

#include <QWidget>
#include <QFile>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMessageBox>
#include <QPixmap>
#include <QTransform>
#include <QTimer>
#include <QListWidget>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QAction>
#include <QMouseEvent>
#include "flatui.h"
#include "rotatinglabel.h"
#include "mydataprocessing.h"

namespace Ui {
class MyMusicWidget;
}

class MyMusicWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyMusicWidget(QWidget *parent = nullptr);
    static MyMusicWidget* GetKernal();
    void initForm();                            /*初始化界面*/
    void loadMedia(const QString& filePath);    /*从指定路径中加载文件*/
    ~MyMusicWidget();

public:
    void initAction();
    inline QString MenuStyle();
    inline QString PlayStyle();
    inline QString PaseStyle();
    inline QString IlikeStyle();
    inline QString Ilike1Style();
    inline QString ShowListStyle();
    inline QString HideListStyle();
    inline QString RandomStyle();
    inline QString LoopStyle();
    inline QString LoopOneStyle();
    inline QString VoiceStyle();
    inline QString NoVoiceStyle();
    inline QString MusicListStyle();
signals:
    void timeout();

private slots:
    void transformPicutrue();
    /*自定义槽函数*/
    void onStateChanged(QMediaPlayer::State _state);

    void onPlaylistChanged(int position);

    void onDurationChanged(qint64 duration);

    void onPositionChanged(qint64 position);

    void on_btnPrevious_clicked();

    void on_btnPlay_clicked();

    void on_btnNext_clicked();

    void on_sliderPosition_valueChanged(int value);

    void on_sliderVolumn_valueChanged(int value);

    void on_btnMode_clicked();

    void needleAnimationDynamic();

    void on_btnMusic_clicked();

    void on_btnRepeat_clicked();

public slots:
    /*接受关注度数值的信号槽函数*/
    void reciveAttention(double attenDataValue);


private:
    Ui::MyMusicWidget *ui;
    static MyMusicWidget*         Instance;                //单例类指针
    MyDataProcessing*             dataProcessing = nullptr;//数据处理类
    QMediaPlayer*                 player;                  //播放器
    QMediaPlaylist*               playlist;                //播放列表控件 （Qt6弃用）
    QMediaPlayer::State           state;                   //播放器状态
    QMediaPlaylist::PlaybackMode  playMode;                //播放器模式
    int                           m_IsMode= 0;             //判断播放模式:0单曲1循环2单曲循环3随机播放
    QString                       durationTime;            //总长度
    QString                       positionTime;            //当前播放到位置
    qint64                        MusicDuration = 0;
    qint64                        MusicPostion = 0;
    QString                       mp3Text;                 //MP3文件名
    QPixmap*                      pix;                     //图片指针
    QPixmap*                      picrneedle;
    QTimer*                       timer;                   //定时器指针
    QTimer*                       recordTimer;
    QTimer*                       synTimer;                //定时调用槽
    int                           m_currentIndex = 0;      //当前播放的音乐
    RotatingLabel *               recordLabel;
    qreal                         recordRotation;
    bool                          m_bPlay {true};
    QVector<double>               mCurrentBuffer;           //关注度缓冲区
    QVector<double>               mNextBuffer;              //刷新后缓冲区
    //QMutex                        m_mutex;                //互斥锁
    double                        m_averageValue;           //平均值
    QAction *                     action5;                   //皮肤设置
    QAction *                     action5_1;                 //默认皮肤1
    QAction *                     action5_2;                 //默认皮肤2
    QAction *                     action5_4;
    QAction *                     action5_3;                 //自定义皮肤
    QPushButton*                  btn1;
    QPushButton*                  btn2;
    QPushButton*                  btn3;
    QPushButton*                  btnPull;

};


#endif // MYMUSICWIDGET_H
