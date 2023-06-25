#include "myMultiCurvesPlot.h"
#include<QDebug>
#pragma execution_character_set("utf-8")


myMultiCurvesPlot::myMultiCurvesPlot(int _curvesCnt, QWidget *parent)
    :QCustomPlot(parent),
    allCurvesData(_curvesCnt),
    curvesCnt(allCurvesData.size())
{
    this->setInteractions(QCP::iRangeDrag //可平移
                          | QCP::iRangeZoom //可滚轮缩放
                          //| QCP::iSelectPlottables //可选中曲线
                          | QCP::iSelectLegend );//可选中图例
    this->setNoAntialiasingOnDrag(true);//禁用抗锯齿，以提高性能
    this->legend->setVisible(true);//使能图例
    this->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop|Qt::AlignLeft);//设置图例的位置
    this->yAxis2->setVisible(true);
    connect(yAxis, SIGNAL(rangeChanged(QCPRange)), yAxis2, SLOT(setRange(QCPRange)));//左右y轴同步放缩

    QSharedPointer<QCPAxisTickerTime>timeTicker(new QCPAxisTickerTime);//时间作x轴
    timeTicker->setTimeFormat("%h:%m:%s");
    this->xAxis->setTicker(timeTicker);
    this->axisRect()->setupFullAxesBox();

    getColor.resize(curvesCnt);
    for(int i = 0; i < getColor.size();i++)
    {
        getColor[i] = QColor(rand()%255, rand()%255, rand()%255);//生成随机颜色
    }
    getName.resize(curvesCnt);
    for(int i = 0; i < getName.size();i++)//设置默认曲线名称
    {
        getName[i] = QString("curve %1").arg(i);
    }

    for(int i = 0; i < allCurvesData.size();i++)//清空备份的数据源
    {
        allCurvesData[i].keyVec.clear();
        allCurvesData[i].valVec.clear();
    }
}


myMultiCurvesPlot::~myMultiCurvesPlot()
{

}

void myMultiCurvesPlot::addData(int curveIdx, double x, double y)
{
    if(curveIdx>allCurvesData.size())
    {
        qDebug()<<QString("warning:下位机企图绘制编号为%1的曲线！").arg(curveIdx);
        return;
    }
    allCurvesData[curveIdx].keyVec.append(x);
    allCurvesData[curveIdx].valVec.append(y);
    if(curveIdx2graphPtr.contains(curveIdx))//第curveIdx个曲线正在显示中
    {
        QCPGraph*pGraph = curveIdx2graphPtr[curveIdx];
        pGraph->addData(x,y);
        pGraph->data().data()->removeBefore(0);
    }
}

void myMultiCurvesPlot::setColors(QVector<QColor> _colors)
{
    if(_colors.size() != getColor.size())
    {
        qDebug() << "warning: MultiCurvesPlot::setColors failed!";
        return;
    }

    getColor = _colors;
}

void myMultiCurvesPlot::showCurves(QList<uint16_t> _idxList)
{
    this->clearGraphs();//先移除所有的graph
    idxList = idxList;//备份，可能别的函数需要它
    curveIdx2graphPtr.clear();//清除曲线编号->graph指针的映射
    graphPtr2curveIdx.clear();//清除graph指针->曲线编号的映射
    int graphIdx = 0;
    for(QList<uint16_t>::const_iterator it = idxList.begin(); it != idxList.end(); it++)
    {   //*it为每一个要显示的曲线编号
        uint16_t curveIdx = *it;
        if(curveIdx > allCurvesData.size())
        {
            qDebug() << QString("warning: MultiCurvesPlot::showCurves->超出数据源max index").arg(*it);
                continue;
        }
        this->addGraph(this->xAxis, this->yAxis);
        QCPGraph* pGraph = graph(graphIdx);
        curveIdx2graphPtr[curveIdx] = pGraph;//记录：曲线索引->graph指针的映射
        graphPtr2curveIdx[pGraph] = curveIdx;//记录：graph指针->曲线索引的映射
        pGraph->setData(allCurvesData[curveIdx].keyVec, allCurvesData[curveIdx].valVec, true);//数据源 （todo:*it的合法性）
        pGraph->setPen(QPen(getColor[curveIdx]));//线的颜色随机
        pGraph->setLineStyle(QCPGraph::lsStepLeft);//阶梯线样式
        this->graph(graphIdx)->setName(getName[curveIdx]);
        graphIdx++;
    }
    this->replot();
}











