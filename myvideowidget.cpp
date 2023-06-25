#include <QKeyEvent>
#include <QMouseEvent>
#include "myvideowidget.h"
#include "ui_myvideowidget.h"

MyVideoWidget *MyVideoWidget::m_Instance=nullptr;

MyVideoWidget::MyVideoWidget(QWidget *parent) :
    QVideoWidget(parent),
    ui(new Ui::MyVideoWidget)
{
    ui->setupUi(this);
}

MyVideoWidget *MyVideoWidget::getInstance()
{
    if(m_Instance==nullptr)
    {
        m_Instance=new  MyVideoWidget();
    }
    return m_Instance;

}

void MyVideoWidget::setMediaPlayer(QMediaPlayer *player)
{
    m_player = player;
}

MyVideoWidget::~MyVideoWidget()
{
    delete ui;
}
