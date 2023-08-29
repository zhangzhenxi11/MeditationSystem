#ifndef MYVIDEOWIDGET_H
#define MYVIDEOWIDGET_H

#include <QWidget>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMouseEvent>
/*
 * 此类只提供video的组件，
 * 实际视频功能myslidewidget中。
*/

namespace Ui {
class MyVideoWidget;
}

class MyVideoWidget : public QVideoWidget
{
    Q_OBJECT

private:
    QMediaPlayer    *m_player;//视频播放器

protected:
    void keyPressEvent(QKeyEvent*event);
public:
    explicit MyVideoWidget(QWidget *parent = nullptr);

    static  MyVideoWidget* getInstance();

    void    setMediaPlayer(QMediaPlayer *player);

    ~MyVideoWidget();

private:
    Ui::MyVideoWidget *ui;
    static MyVideoWidget* m_Instance;//类指针

};
#endif // MYVIDEOWIDGET_H
