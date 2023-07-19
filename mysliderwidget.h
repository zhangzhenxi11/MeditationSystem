#ifndef MYSLIDERWIDGET_H
#define MYSLIDERWIDGET_H

#include <QWidget>
#include <QPropertyAnimation> //动画类
#include <QParallelAnimationGroup>//动画组类
#include <QStackedWidget>//用于存储多个界面
#include <QPushButton>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QMouseEvent>
#include <QPoint>
#include <QScrollArea>              // 滚动条
#include <QAudioDecoder>            // 解码本地音频媒体文件
#include <QAudioEncoderSettings>    // 音频编码器设置对象
#include <QProcess>
#include "myvideowidget.h"
#include "sdk/flatui.h"
#include <QProcess>
// ffmpeg 音视频库
extern "C"{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
}
/*
1、按钮的槽函数是改变列表行号，切换视频。
2、视频播放的部件是对应的qvideowidget。
3、切换按钮时，停止前一个视频，开始播放下一个视频
4、屏蔽点原视频音乐，播放自定音乐
5、程序自动加载音乐
*/

namespace Ui {
class mySliderWidget;
}

class mySliderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit mySliderWidget(QWidget *parent = nullptr);

    void loadMedia(const QString& videoFilePath,const QString & audioFilePath);              /* 从指定路径中加载文件 */

    void initForm();                                      /* 初始化界面 */

    static mySliderWidget* GetKernel();                   /* 单例接口 */

    ~mySliderWidget();

protected slots:
    //自定义槽函数
    void onAnimationFinished();

    void onstateChanged(QMediaPlayer::State _state);

    void onDurationChanged(qint64 duration);

    void onPositionChanged(qint64 position);

    bool eventFilter(QObject *obj, QEvent *event) override;

    void switchToNextVideo();//切换下一个视频

    void switchToPreviousVideo(); //切换上一个视频

    void switchToVideo(int index);//切换视频

    void animateVideoLabel();//动画

    void onSwitcnPreSong();

    void onSwitcnNextSong();

    void mergeVideoWithAudio(const QString& videoFilePath, const QString& audioFilePath, const QString& outputFilePath);

private slots:

    void on_btnPlay_clicked();

    void on_btnSound_clicked();

    void on_sliderVolumn_valueChanged(int value);

    void on_sliderPosition_valueChanged(int value);

    void on_btnPreviousSong_clicked();

    void on_btnNextSong_clicked();

private:
    Ui::mySliderWidget  *               ui;
    static mySliderWidget  *            m_instance;
    QMediaPlayer  *                     mVideoPlayer;        //视频播放器
    QMediaPlaylist  *                   playlist;            //播放列表
    QMediaPlayer::State                 state;               //播放器状态
    QMediaPlaylist::PlaybackMode        playMode;            //播放器模式
    int                                 m_IsMode = 0;        //判断播放模式   0单曲1循环2单曲循环3随机播放
    QStringList                         m_fileList;          //视频文件列表
    QStringList                         m_videoNameList;     //视频文件名列表
    QMap<QString,QString>               m_videoCombinaAudio; //视频音频文件地址绑定
    QString                             durationTime;        //进度间隔
    QString                             positionTime;        //进度位置
    QStackedWidget  *                   m_ptrStackWidget;    //分页stacke
    MyVideoWidget   *                   m_videoWidegt;
    QHBoxLayout     *                   m_ptrLayoutMain;
    QPushButton     *                   m_ptrBtnPre;
    QPushButton     *                   m_ptrBtnNext;
    int                                 m_windowWidth ;
    int                                 m_windowHieght;
    QMap<int, MyVideoWidget*>           m_indexToWidget;     //组件和视频绑定
    int                                 m_rowNo;             //行号
    int                                 m_videoNums =3;
    bool                                m_bDonghua = false;
    int                                 m_flagPreOrNext=0;   //0:👈划 1：👉划
    int                                 m_currentIndex = 0;
    int                                 m_currentVideoIndex=0;//video索引
    QPropertyAnimation *                m_manimation;         //动画
    bool                                mousePressed = false;
    QPoint                              mousePos;             //鼠标位置点
};


#endif // MYSLIDERWIDGET_H
