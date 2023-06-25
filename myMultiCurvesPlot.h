/*
QCustomplot的绘图机制：每条曲线都要绘制在一个单独的graph中

*/
#ifndef MYMULTICURVESPLOT_H
#define MYMULTICURVESPLOT_H

#endif // MYMULTICURVESPLOT_H
#include <QObject>
#include "qcustomplot.h"
#include <QMap>
#include <QDebug>
typedef struct _CurveData
{
    QVector<double> keyVec;//x
    QVector<double> valVec;//y
}CurveData;//单个曲线的数据

class myMultiCurvesPlot: public QCustomPlot
{
    Q_OBJECT
public:
    myMultiCurvesPlot(int _curvesCnt, QWidget *parent = 0);
    ~myMultiCurvesPlot();

    void setCurvesName(QVector<QString> _nameVec);

    void addData(int curveIdx, double x, double y);

    void setColors(QVector<QColor> _colors);

public slots:

    void showCurves(QList<uint16_t> _idxList);

private:
    QVector<CurveData> allCurvesData;//所有曲线的数据,allCurvesData[n]中又含有两个QVector，也即第n条曲线的x、y值

    int curvesCnt;//支持的曲线数目

    QMap<int, QCPGraph*> curveIdx2graphPtr;//记录或查询：从曲线索引(key)->graph指针(val)。

    QMap<QCPGraph*, int> graphPtr2curveIdx;//记录或查询：从graph指针(key)->曲线索引(val)。

    QVector<QColor> getColor;//记录所有曲线的颜色

    QVector<QString> getName;  //记录所有曲线的名称（图例）

    QList<uint16_t> idxList;//当前正在显示的曲线的编号

    QAction *actClearDatas;//右键菜单:清空历史数据

    QAction *actshowAllGraph;//右键菜单:全显
};



