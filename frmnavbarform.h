#ifndef FRMNAVBARFORM_H
#define FRMNAVBARFORM_H

#include <QWidget>
#include <QPainterPath>
#include <QTabWidget> //选项卡式的窗口控件
#include <QThread>    //多线程
#include <QHBoxLayout>

namespace Ui
{
class frmNavBarForm;
}
// 左侧是自定义控件组navbar  右侧是stackedWidget分页控件
class frmNavBarForm : public QWidget
{
	Q_OBJECT

public:
    explicit frmNavBarForm(QWidget *parent = 0);
    ~frmNavBarForm();


protected:
    void showEvent(QShowEvent *);

public slots:
    void receiveData(int index);
private:
    Ui::frmNavBarForm *ui;
    QWidget* m_musicWidet = nullptr ;     //新插入的页面1
    QWidget* m_videoWidegt = nullptr;
    QHBoxLayout * m_LayoutMain = nullptr ;//主页面布局

private slots:
	void initForm();
    void currentItemChanged(int index, const QString &item);
    void setIndex();
};

#endif // FRMNAVBARFORM_H
