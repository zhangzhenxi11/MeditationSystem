#ifndef THEMESELECTION_H
#define THEMESELECTION_H

#include <QWidget>
#include <QLabel>
#include <QFile>
#include <imageview.h>
#include <QPainterPath>

namespace Ui {
class ThemeSelection;
}

class ThemeSelection : public QWidget
{
    Q_OBJECT

public:
    explicit ThemeSelection(QWidget *parent = nullptr);
    void initUiForm();  //加载资源文件
    ~ThemeSelection();

private slots:
    void on_btnSky_clicked();

    void on_btnNoise_clicked();

    void on_btnSlient_clicked();

private:
    Ui::ThemeSelection *     ui;
    QString                  m_styleSheet;
    QString                  m_themeName;
    int                      m_SourceNum;
    QString                  m_strFolder;

};

#endif // THEMESELECTION_H
