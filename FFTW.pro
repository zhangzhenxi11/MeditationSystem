QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport serialport multimedia  multimediawidgets

CONFIG += c++17

CONFIG += resources_big

INCLUDEPATH += ../FFTW/eigen-3.4.0

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    bandpassfilter.cpp \
    comservice.cpp \
    flatui.cpp \
    frmnavbarform.cpp \
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
    rotatinglabel.cpp



HEADERS += \
    LowPassFilter.h \
    bandpassfilter.h \
    comservice.h \
    flatui.h \
    frmnavbarform.h \
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
    rotatinglabel.h


FORMS += \
    frmnavbarform.ui \
    mycustomplotwidget.ui \
    mymusicwidget.ui \
    myserialwidget.ui \
    mysliderwidget.ui \
    myvideowidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include($$PWD/gsl/gsl.pri)

#编译源码文件太大
QMAKE_CXXFLAGS += "-Wa,-mbig-obj"

RESOURCES += \
    Qss.qrc \
    Resource.qrc \
    images.qrc \
    res.qrc

RC_FILE = myico.rc


#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/sdk/lib/ -lquc
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/sdk/lib/ -lqucd
#else:unix: LIBS += -L$$PWD/sdk/lib/ -lquc

#INCLUDEPATH += $$PWD/sdk
#DEPENDPATH += $$PWD/sdk
