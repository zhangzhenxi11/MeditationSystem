#include "themeselection.h"
#include "ui_themeselection.h"
#include <QPushButton>
#include <QButtonGroup>
#include <QDebug>

ThemeSelection::ThemeSelection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ThemeSelection)
{
    ui->setupUi(this);
    this->initForm();

    QPushButton* btnMusic= new QPushButton("视频1",this);
    btnMusic->setGeometry(180,110,181,281);
    btnMusic->setFont(QFont("Microsoft YaHei UI"));
    btnMusic->setStyleSheet("color: rgb(85, 170, 255)");

    QPushButton* btnSerial= new QPushButton("视频2",this);
    btnSerial->setGeometry(550,110,181,281);
    btnSerial->setFont(QFont("Microsoft YaHei UI"));
    btnSerial->setStyleSheet("color: rgb(85, 170, 255)");

    connect(btnMusic,&QPushButton::clicked,this,&ThemeSelection::onbtnMusic);
    connect(btnSerial,&QPushButton::clicked,this,&ThemeSelection::onbtnSerial);

}
//ThemeSelection*ThemeSelection::m_instance = nullptr;
//ThemeSelection *ThemeSelection::GetKernl()
//{
//    if(m_instance== nullptr)
//    {
//        m_instance = new ThemeSelection();
//    }
//    return m_instance;
//}

void ThemeSelection::onbtnMusic()
{

    emit emitCurrentItem(1);
    qDebug()<<"emit signal finished!";
}

void ThemeSelection::onbtnSerial()
{
    emit emitCurrentItem(2);
    qDebug()<<"emit signal finished!";
}

void ThemeSelection::onStart()
{

}

ThemeSelection::~ThemeSelection()
{
//    delete ui;
}

void ThemeSelection::initForm()
{
    QFile file(":/Qss/Qss.qss");
    file.open(QFile::ReadOnly);
    if (file.isOpen())
    {
        this->setStyleSheet(file.readAll());
    }
    file.close();
//    QLabel * SubLabel = new QLabel(this);
//    SubLabel->setGeometry(QRect(0,0,1080,60));
//    SubLabel->setText("主题选择");
//    SubLabel->setObjectName("SubLabel");
//    SubLabel->setAlignment(Qt::AlignCenter);

}
