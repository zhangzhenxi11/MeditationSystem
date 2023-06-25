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
#include "flatui.h"
#include "rotatinglabel.h" //自定义lable

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

private:
    Ui::MyMusicWidget *ui;
    static MyMusicWidget*         Instance;    //单例类指针
    QMediaPlayer*                 player;      //播放器
    QMediaPlaylist*               playlist;    //播放列表控件 （Qt6弃用）
    QMediaPlayer::State           state;       //播放器状态
    QMediaPlaylist::PlaybackMode  playMode;    //播放器模式
    int                           m_IsMode= 0; //判断播放模式:0单曲1循环2单曲循环3随机播放
    QString                       durationTime;//总长度
    QString                       positionTime;//当前播放到位置
    QString                       mp3Text;     //MP3文件名
    QPixmap*                      pix;         //图片指针
    QPixmap*                      picrneedle;
    QTimer*                       timer;       //定时器指针
    int                           m_currentIndex = 0; //当前播放的音乐
    QMatrix                       matrix;
    QTimer*                       recordTimer;
    RotatingLabel *               recordLabel;
    qreal                         recordRotation;
    bool                          m_bPlay {true};
};


#endif // MYMUSICWIDGET_H
