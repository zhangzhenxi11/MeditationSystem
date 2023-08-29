#include "themeselection.h"
#include "qplaintextedit.h"
#include "ui_themeselection.h"
#include <QPushButton>
#include <QDebug>
#include <QFile>
#include <QDir>

/*
开发主题界面功能：
1、背景是静态图片,点击按钮切换背景图片。
2、有动画效果，背景与音乐绑定。
3、背景图片上方有三个按钮选项，切换背景图片分类
*/
ThemeSelection::ThemeSelection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ThemeSelection)
{
    ui->setupUi(this);
    this->initUiForm();
}

void ThemeSelection::initUiForm()
{
    QString styleSheet1 = QString("QPushButton{"
                                  "background:rgba(255,255,0,0);"
                                  "border-image: url(:/images/play.png);"
                                  "})");

    ui->btnPlay->setIconSize(QSize(90,90));//重置图标大小
    ui->btnPlay->setStyleSheet(styleSheet1);
    //imageView显示图片
    m_strFolder = "../FFTW/images/天籁";
    ui->widget->load(m_strFolder);
}


ThemeSelection::~ThemeSelection()
{
    delete ui;
}

void ThemeSelection::on_btnSky_clicked()
{
    m_strFolder = "../FFTW/images/天籁";
    ui->widget->load(m_strFolder);
}


void ThemeSelection::on_btnNoise_clicked()
{
    m_strFolder = "../FFTW/images/遮燥";
    ui->widget->load(m_strFolder);
}

void ThemeSelection::on_btnSlient_clicked()
{
    m_strFolder = "../FFTW/images/静谧";
    ui->widget->load(m_strFolder);
}




