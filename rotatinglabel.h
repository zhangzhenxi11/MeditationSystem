
#ifndef ROTATINGLABEL_H
#define ROTATINGLABEL_H


#include <QObject>
#include <QLabel>
#include <QTimer>

class RotatingLabel:public QLabel
{
    Q_OBJECT
public:
    RotatingLabel(QWidget *parent = nullptr);

    void setRotationAngle(int angle);

    void updateLable();

protected:
    void paintEvent(QPaintEvent*event);

private:
    int rotationAngle;
    QPixmap         Pixmap;

};

#endif // ROTATINGLABEL_H
