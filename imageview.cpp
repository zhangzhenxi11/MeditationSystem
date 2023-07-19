#pragma execution_character_set("utf-8")

#include "imageview.h"
#include "qevent.h"
#include "qpainter.h"
#include <QPainterPath>
#include "qtoolbutton.h"
#include "qtimer.h"
#include "qaction.h"
#include "qdir.h"
#include "qfiledialog.h"
#include "qdebug.h"

ImageNum::ImageNum(QWidget *parent) : QWidget(parent)
{
    totalNum = 0;
    currentIndex = -1;
#ifdef __arm__
    setFixedSize(250, 35);
#else
    setFixedSize(180, 30);
#endif
}

void ImageNum::setDrawRect(const QRect &rect)
{
    m_rect = rect;
}

void ImageNum::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
//    drawBg(&painter);
    if(m_rect.top()== 240)
    {
        drawText(&painter);
    }
    else if(m_rect.top()== 200)
    {
        drawNameText(&painter);
    }
}

void ImageNum::drawBg(QPainter *painter)
{
    painter->save(); //保存QPainter的所有状态 save会把状态入栈
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(46, 132, 243));
    painter->drawRoundedRect(rect(), 2, 2);
    painter->restore();// 把QPainter恢复到最近一次save()时的状态,即出栈,恢复到save前的状态
}

void ImageNum::drawText(QPainter *painter)
{
    QString strText = QString("第 %1 张 / 共 %2 张").arg(currentIndex + 1).arg(totalNum);
    painter->save();
    painter->setPen(Qt::white);
    QFont textFont;
    textFont.setBold(true);
    painter->setFont(textFont);
    painter->drawText(rect(), Qt::AlignCenter, strText);
    painter->restore();
}

void ImageNum::drawNameText(QPainter * painter)
{
    QString strNameText = m_text;
    painter->save();
    painter->setPen(Qt::white);
    QFont textFont;
    textFont.setBold(true);
    painter->setFont(textFont);
    painter->drawText(rect(), Qt::AlignCenter, strNameText);
    painter->restore();
}

void ImageNum::setTotalNum(int totalNum)
{
    if (totalNum < 0) {
        this->totalNum = 0;
    } else {
        this->totalNum = totalNum;
    }

    this->currentIndex = 0;
    this->update();//自动调用paintEvent()函数
}

void ImageNum::setCurrentIndex(int currentIndex)
{
    if (currentIndex < 0) {
        this->currentIndex = -1;
    } else {
        this->currentIndex = currentIndex;
    }

    this->update();
}

void ImageNum::setCurrentName(QString currentName)
{
    this->m_text = currentName;
    this->update();
}

ImageView::ImageView(QWidget *parent) :	QWidget(parent)
{
    setStyleSheet(".QToolButton{background-color:transparent;border:none;background: rgba(255,255,0,0);}");

    bgColorStart = QColor(100, 100, 100);
    bgColorEnd = QColor(60, 60, 60);

    bottomSpace = 10;
    buttonSpace = 10;
    icoSize = QSize(65, 65);

    fade = true;
    fill = true;
    keyMove = false;

    totalNum = 0;
    currentIndex = -1;
//    QString(ImageName); //空字符串

    num = new ImageNum(this);
    QRect rect1(420, 240, 200, 50);
    num->setDrawRect(rect1);

    picutreName = new ImageNum(this);   // ImageNum类指针，构造会再次绘制一个矩形
    QRect rect2(420, 200, 200, 50);
    picutreName->setDrawRect(rect2);

    connect(this, SIGNAL(totalNumChanged(int)), num, SLOT(setTotalNum(int)));
    connect(this, SIGNAL(currentIndexChanged(int)), num, SLOT(setCurrentIndex(int)));
    connect(this, SIGNAL(currentNameChanged(QString)),picutreName,SLOT(setCurrentName(QString)));

    preButton = new QToolButton(this);
    nextButton = new QToolButton(this);

    preButton->setIconSize(icoSize);
    nextButton->setIconSize(icoSize);

    preButton->setStyleSheet("border-image: url(:/images/pre.png);background: rgba(255,255,0,0);");
    nextButton->setStyleSheet("border-image: url(:/images/next.png);background: rgba(255,255,0,0);");

    connect(preButton, SIGNAL(clicked()), this, SLOT(movePrevious()));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(moveNext()));

    opacity = 1.0;
    timer = new QTimer(this);
    timer->setInterval(50);
    connect(timer, SIGNAL(timeout()), this, SLOT(doFading()));

    QAction *action_load = new QAction("载入", this);
    connect(action_load, SIGNAL(triggered(bool)), this, SLOT(load()));
    this->addAction(action_load);

    QAction *action_clear = new QAction("清空", this);
    connect(action_clear, SIGNAL(triggered(bool)), this, SLOT(clear()));
    this->addAction(action_clear);

    this->setContextMenuPolicy(Qt::ActionsContextMenu);
    calcGeo();
}

ImageView::~ImageView()
{
    if (timer->isActive()) {
        timer->stop();
    }
}

void ImageView::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    drawBg(&painter);

    if (totalNum > 0) {
        drawImage(&painter);
    }
}

void ImageView::drawBg(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    QLinearGradient bgGradient(QPoint(0, 0), QPoint(0, height()));
    bgGradient.setColorAt(0.0, bgColorStart);
    bgGradient.setColorAt(1.0, bgColorEnd);
    painter->setBrush(bgGradient);
    painter->drawRect(rect());
    painter->restore();
}

void ImageView::drawImage(QPainter *painter)
{
    painter->save();
    painter->setOpacity(opacity);

    if (fill) {
        painter->drawImage(rect(), currentImage);
        painter->restore();
    } else {
        //按照比例自动居中绘制
        int imageWidth = currentImage.width();
        int imageHeight = currentImage.height();
        int imageX = rect().center().x() - imageWidth / 2;
        int imageY = rect().center().y() - imageHeight / 2;
        QPoint point(imageX, imageY);

        painter->drawImage(point, currentImage);
        painter->restore();
    }
}

void ImageView::keyPressEvent(QKeyEvent *keyEvent)
{

    if (keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Up) {
        movePrevious();
    } else if (keyEvent->key() == Qt::Key_Right || keyEvent->key() == Qt::Key_Down) {
        moveNext();
    }
}

void ImageView::resizeEvent(QResizeEvent *)
{
    calcGeo();
}

void ImageView::showEvent(QShowEvent *)
{
    calcGeo();
}

void ImageView::calcGeo()
{
    QPoint prePoint(buttonSpace, (height() - preButton->height()) / 2);
    preButton->move(prePoint);

    QPoint nextPoint(width() - buttonSpace - preButton->width(), (height() - preButton->height()) / 2);
    nextButton->move(nextPoint);

//    QPoint numPoint(width() / 2 - num->width() / 2, height() - bottomSpace - preButton->height() / 2 - num->height() / 2);
    QPoint numPoint(420,240);
    num->move(numPoint);

//    QPoint namePoint(429, 490);
    QPoint namePoint(420, 200);
    picutreName->move(namePoint);
}

void ImageView::doFading()
{
    opacity += 0.05;

    if (opacity > 1.0) {
        opacity = 1.0;
        timer->stop();
    }

    this->update();
}

QColor ImageView::getBgColorStart() const
{
    return this->bgColorStart;
}

QColor ImageView::getBgColorEnd() const
{
    return this->bgColorEnd;
}

int ImageView::getBottomSpace() const
{
    return this->bottomSpace;
}

int ImageView::getButtonSpace() const
{
    return this->buttonSpace;
}

QSize ImageView::getIcoSize() const
{
    return this->icoSize;
}

bool ImageView::getFill() const
{
    return this->fill;
}

bool ImageView::getFade() const
{
    return this->fade;
}

bool ImageView::getKeyMove() const
{
    return this->keyMove;
}

QSize ImageView::sizeHint() const
{
    return QSize(500, 350);
}

QSize ImageView::minimumSizeHint() const
{
    return QSize(10, 10);
}

void ImageView::load()
{
    QString strDir = QFileDialog::getExistingDirectory(this, "选择图片文件");

    if (!strDir.isEmpty()) {
        load(strDir);
    }
}

void ImageView::load(const QString &strFolder)
{
    QDir imageFolder(strFolder);
    //按照文件名称降序排序
    imageFolder.setSorting(QDir::Name | QDir::Reversed);

    if (imageFolder.exists()) {
        QStringList filter;
        filter << "*.png" << "*.jpg" << "*.gif" << "*.jpeg" << "*.bmp" ;
        imageNames = imageFolder.entryList(filter);

        totalNum = imageNames.count();
        currentIndex = -1;
        imagePathList.clear();

        if (totalNum > 0) {
            foreach (QString strImage, imageNames) {
                QString strFullPath = QString("%1/%2").arg(strFolder).arg(strImage);
                imagePathList.append(strFullPath);
                currentImageName = strImage ;
                emit currentNameChanged(currentImageName);
            }
            moveTo(0);
            emit totalNumChanged(totalNum);
        }
    }
}

void ImageView::clear()
{
    totalNum = 0;
    currentIndex = -1;
    imagePathList.clear();
    this->update();

    emit totalNumChanged(totalNum);
    emit currentIndexChanged(currentIndex);
}

void ImageView::setBgColorStart(const QColor &bgColorStart)
{
    if (this->bgColorStart != bgColorStart) {
        this->bgColorStart = bgColorStart;
        this->update();
    }
}

void ImageView::setBgColorEnd(const QColor &bgColorEnd)
{
    if (this->bgColorEnd != bgColorEnd) {
        this->bgColorEnd = bgColorEnd;
        this->update();
    }
}

void ImageView::setBottomSpace(int bottomSpace)
{
    if (this->bottomSpace != bottomSpace) {
        this->bottomSpace = bottomSpace;
        this->update();
    }
}

void ImageView::setButtonSpace(int buttonSpace)
{
    if (this->buttonSpace != buttonSpace) {
        this->buttonSpace = buttonSpace;
        this->update();
    }
}

void ImageView::setIcoSize(const QSize &icoSize)
{
    if (this->icoSize != icoSize) {
        this->icoSize = icoSize;
        preButton->setIconSize(icoSize);
        nextButton->setIconSize(icoSize);
        calcGeo();
        this->update();
    }
}

void ImageView::setFill(bool fill)
{
    if (this->fill != fill) {
        this->fill = fill;
        this->update();
    }
}

void ImageView::setFade(bool fade)
{
    if (this->fade != fade) {
        this->fade = fade;
        this->update();
    }
}

void ImageView::setKeyMove(bool keyMove)
{
    if (this->keyMove != keyMove) {
        this->keyMove = keyMove;

        if (keyMove) {
            setFocusPolicy(Qt::StrongFocus);
        } else {
            setFocusPolicy(Qt::NoFocus);
        }
    }
}

void ImageView::moveFirst()
{
    int index = 0;

    if (currentIndex != index) {
        moveTo(index);
    }
}

void ImageView::moveLast()
{
    int index = totalNum - 1;

    if (currentIndex != index) {
        moveTo(index);
    }
}

void ImageView::movePrevious()
{
    if (currentIndex > 0) {
        currentIndex--;
        moveTo(currentIndex);
    }
}

void ImageView::moveNext()
{
    if (currentIndex < totalNum - 1) {
        currentIndex++;
        moveTo(currentIndex);
    }
}

void ImageView::moveTo(int index)
{
    if (totalNum == 0 || index > totalNum - 1) {
        return ;
    }
    currentIndex = index;
    currentImage = QImage(imagePathList.at(currentIndex));
    currentImageName = imageNames.at(currentIndex).split('.').at(0);
    emit currentIndexChanged(currentIndex);
    emit currentNameChanged(currentImageName);

    if (fade) {
        opacity = 0.0;
        timer->start();
    } else {
        this->update();
    }
}
