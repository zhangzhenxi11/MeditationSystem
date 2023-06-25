#ifndef THEMESELECTION_H
#define THEMESELECTION_H

#include <QWidget>
#include <QLabel>
#include <QFile>

namespace Ui {
class ThemeSelection;
}

class ThemeSelection : public QWidget
{
    Q_OBJECT

public:
    explicit ThemeSelection(QWidget *parent = nullptr);
    void onbtnMusic();
    void onbtnSerial();
    void onStart();
    ~ThemeSelection();

signals:
    void emitCurrentItem(int index);

private slots:
    void initForm();

private:
    Ui::ThemeSelection *ui;
};

#endif // THEMESELECTION_H
