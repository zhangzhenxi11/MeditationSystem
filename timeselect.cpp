#include "timeselect.h"
#include "ui_timeselect.h"

TimeSelect::TimeSelect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimeSelect)
{
    ui->setupUi(this);
}

TimeSelect::~TimeSelect()
{
    delete ui;
}
