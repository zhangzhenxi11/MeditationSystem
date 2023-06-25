
#ifndef MYTHREAD_H
#define MYTHREAD_H


#include <QObject>
#include <QThread>
#include <QDebug>
/*
 * 采用movethread开始多线程
   滤波算法和绘制曲线

1.数据源是读取文本返回数组
2.低通滤波波  绘制到plot1
3.带通滤波   绘制到plot2

*/
//数据源任务类
class Generate : public QObject
{
    Q_OBJECT
public:
    explicit Generate(QObject *parent = nullptr);

    void working(int num);

signals:
    void sendArray(QVector<double> num);

};

//任务1
class Sourcethread : public QObject
{
    Q_OBJECT
public:
    explicit Sourcethread(QObject *parent = nullptr);

    double iteration_data(QVector<double> &itContainer);

signals:
    void finish(int num);

};

//任务2
class Cleanthread : public QObject
{
    Q_OBJECT
public:
    explicit Cleanthread(QObject *parent = nullptr);

    double iteration_data(QVector<double> &itContainer);

signals:
    void finish(int num);

};
#endif // MYTHREAD_H
