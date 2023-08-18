#include "frmnavbarform.h"
#include "ui_frmnavbarform.h"
#include <QTimer>
#include "qdebug.h"
#include "mysliderwidget.h"
#include "mymusicwidget.h"
#include "LhuierMusicDefine.h"
#include <QFile>

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

void frmNavBarForm::closeEvent(QCloseEvent *event)
{
    QFile file(CUSTOMPLOT_SOURCEFILE_PATH);
    file.open(QFile::WriteOnly|QFile::Truncate);
    file.close();
    QFile outfile(QApplication::applicationDirPath()+ CUSTOMPLOT_OUTFILE_PATH);
    outfile.open(QFile::WriteOnly|QFile::Truncate);
    outfile.close();
    event->accept();
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
    ui->navBar->setItems("冥想界面;用户操作;串口设置;主题选择");
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
    ui->tabWidget->resize(width(), height());
    ui->wMain->setLayout(gridLayout);
    ui->wMain->resize(width(), height());
    setIndex();//默认第一个界面
    //设置子界面垂直布局
//    m_musicWidet->adjustSize();
//    m_videoWidegt->adjustSize();
//    ui->wSerialSet->adjustSize();
//    ui->wMain->adjustSize();
//    QVBoxLayout *subLayout1 = new QVBoxLayout(m_musicWidet);
//    QVBoxLayout *subLayout2 = new QVBoxLayout(m_videoWidegt);

}

void frmNavBarForm::currentItemChanged(int index, const QString &item)
{
    ui->stackedWidget->setCurrentIndex(index);
    qDebug() << "index" << index << "item" << item;


    /* QStackedWidget实现自适应紧凑布局
    每个子窗口里加一个垂直布局，将原本的子窗口内容作为一个content_widget放到新加的布局里。
    在显示当前页面时，隐藏其他页面的content_widget即可
    */

    // 经测试，隐藏page是不行的，需要隐藏page里面的content_widget
//    int page_count = ui->stackedWidget->count();
//    for (int i = 0; i < page_count; i++)
//    {
//        QWidget *page = ui->stackedWidget->widget(i);
//        QObjectList objects = page->children();
//        for (int j = 0; j < objects.size(); j++)
//        {
//            QWidget *content_widget = qobject_cast<QWidget *>(objects.at(j));
//            if (content_widget)
//            {
//                content_widget->setVisible(i == index);
//                break; // 这里只是跳出当前页的for循环
//            }
//        }
//    }


}

void frmNavBarForm::setIndex()
{
    ui->navBar->setCurrentIndex(2);
}


