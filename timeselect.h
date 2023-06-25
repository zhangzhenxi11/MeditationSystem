#ifndef TIMESELECT_H
#define TIMESELECT_H

#include <QWidget>

namespace Ui {
class TimeSelect;
}

class TimeSelect : public QWidget
{
    Q_OBJECT

public:
    explicit TimeSelect(QWidget *parent = nullptr);
    ~TimeSelect();

private:
    Ui::TimeSelect *ui;
};

#endif // TIMESELECT_H
