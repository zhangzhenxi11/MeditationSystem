#include "frmnavbarform.h"
#include "ui_frmnavbarform.h"
#include <QTimer>
#include "qdebug.h"
#include "mysliderwidget.h"
#include "mymusicwidget.h"
#include <QTimer>

frmNavBarForm::frmNavBarForm(QWidget *parent) : QWidget(parent),
    ui(new Ui::frmNavBarForm)
{
    ui->setupUi(this);
    this->initForm();
}

frmNavBarForm::~frmNavBarForm()
{
    delete ui;
}

void frmNavBarForm::showEvent(QShowEvent *)
{
    QTimer::singleShot(100, this, SLOT(setIndex()));
}

void frmNavBarForm::receiveData(int index)
{
    if(index == 0)
        currentItemChanged(index,"冥想界面");
    else if(index == 3)
        currentItemChanged(index,"串口设置");
}

void frmNavBarForm::initForm()
{
    //设置圆角
    this->setWindowTitle("冥想系统");
    ui->navBar->setBgRadius(5);
    ui->navBar->setBarRadius(5);
    //设置间距,即元素高度
    ui->navBar->setSpace(18);
    ui->navBar->setBarColorStart(QColor(24, 189, 155));
    ui->navBar->setBarColorEnd(QColor(24, 189, 155));
    ui->navBar->setItems("冥想界面;用户操作;串口设置");
    ui->navBar->setKeyMove(true);
    ui->navBar->setBgColorStart(QColor(46,60,82));
    ui->navBar->setBarColorEnd(QColor(68,47,81));
    connect(ui->navBar, SIGNAL(currentItemChanged(int, QString)), this, SLOT(currentItemChanged(int, QString)));
    //实例化其他界面
    ui->stackedWidget->setMinimumSize(200,300);
    ui->stackedWidget->setMaximumSize(QSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX));
    //插入界面
    m_videoWidegt = mySliderWidget::GetKernel();
    m_musicWidet = MyMusicWidget::GetKernal();

    //tabwidget插入两页
    ui->tabWidget->setTabShape(QTabWidget::Rounded);
    ui->tabWidget->setTabPosition(QTabWidget::North);//上方
    ui->tabWidget->insertTab(0,m_musicWidet,"冥想音乐");
    ui->tabWidget->insertTab(1,m_videoWidegt,"冥想视频");
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(ui->tabWidget);
    ui->tabWidget->resize(1200, 1500);
    ui->wMain->setLayout(gridLayout);
    ui->wMain->resize(1000, 1000);
    setIndex();//默认第一个界面
}

void frmNavBarForm::currentItemChanged(int index, const QString &item)
{
    ui->stackedWidget->setCurrentIndex(index);
    qDebug() << "index" << index << "item" << item;
}

void frmNavBarForm::setIndex()
{
    ui->navBar->setCurrentIndex(2);
}


