
#include "rotatinglabel.h"
#include <QPainter>

RotatingLabel::RotatingLabel(QWidget * parent):
    QLabel(parent),
    rotationAngle(0)
{
    Pixmap.load(":/images/Pole.png");

}

void RotatingLabel::setRotationAngle(int angle)
{
    rotationAngle =angle;
    update();

}

void RotatingLabel::updateLable()
{
    update();
}

void RotatingLabel::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    QPixmap rotatedPixmap = Pixmap.transformed(QTransform().rotate(rotationAngle));
    painter.drawPixmap(rect(), rotatedPixmap);
}

