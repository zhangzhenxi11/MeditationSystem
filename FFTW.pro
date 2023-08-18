QT       += core gui
QT       += concurrent
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport serialport multimedia  multimediawidgets

CONFIG += c++11
CONFIG += c++17
CONFIG += resources_big

# eigen数学库
INCLUDEPATH += ../FFTW/eigen-3.4.0

# ffmpeg 第三方库
INCLUDEPATH += $$PWD/lib/ffmpeg/include

LIBS += -L$$PWD/lib/ffmpeg/lib -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lpostproc -lswresample -lswscale


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    bandpassfilter.cpp \
    comservice.cpp \
    edflib.c \
    flatui.cpp \
    frmnavbarform.cpp \
    imageview.cpp \
    mycustomplotwidget.cpp \
    mydataprocessing.cpp \
    mymusicwidget.cpp \
    myserialwidget.cpp \
    mysliderwidget.cpp \
    myvideowidget.cpp \
    navbar.cpp \
    iir.c \
    main.cpp \
    myfilter/myfilter.cpp \
    qcustomplot.cpp\
    rotatinglabel.cpp \
    themeselection.cpp



HEADERS += \
    LhuierMusicDefine.h \
    LowPassFilter.h \
    bandpassfilter.h \
    comservice.h \
    edflib.h \
    flatui.h \
    frmnavbarform.h \
    imageview.h \
    mycustomplotwidget.h \
    mydataprocessing.h \
    mymusicwidget.h \
    myserialwidget.h \
    mysliderwidget.h \
    myvideowidget.h \
    navbar.h \
    iir.h \
    myfilter/myfilter.h \
    qcustomplot.h\
    rotatinglabel.h \
    themeselection.h


FORMS += \
    frmnavbarform.ui \
    mycustomplotwidget.ui \
    mymusicwidget.ui \
    myserialwidget.ui \
    mysliderwidget.ui \
    myvideowidget.ui \
    themeselection.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# gsl数学库配置文件
include($$PWD/gsl/gsl.pri)

# 编译源码文件太大
QMAKE_CXXFLAGS += "-Wa,-mbig-obj"

RESOURCES += \
    Qss.qrc \
    Resource.qrc \
    images.qrc

RC_FILE = myico.rc

DISTFILES += \
    images/10043.png \
    images/10044.png \
    images/10045.png \
    images/610.bmp \
    images/612.bmp \
    images/620.bmp \
    images/622.bmp \
    images/624.bmp \
    images/632.bmp \
    images/List_loop.png \
    images/Pole.png \
    images/Random_loop.png \
    images/Recycle Bin empty.ico \
    images/Single_cycle.png \
    images/btn_next_normal.png \
    images/btn_pre_normal.png \
    images/danqu.png \
    images/danquxunhuan.png \
    images/ling.ico \
    images/mute-hover.png \
    images/mute.bmp \
    images/mute.png \
    images/next-hover.png \
    images/next.png \
    images/pause-hover.png \
    images/pause.png \
    images/pipe0.png \
    images/play-hover.png \
    images/play.png \
    images/pre-hover.png \
    images/pre.png \
    images/quit.png \
    images/record.png \
    images/singlecycle.png \
    images/styles.css \
    images/suiji.png \
    images/volume-hover.png \
    images/volume.png \
    images/volumn.bmp \
    images/xunhuan.png

