# 宏定义项，指明采用GSL，因为有些函数需要使用dll
DEFINES += GSL_DLL
INCLUDEPATH += $$PWD\include
LIBS += -L$$PWD\lib -llibgsl
LIBS += -L$$PWD\lib -llibgsl.dll
LIBS += -L$$PWD\lib -llibgsl
LIBS += -L$$PWD\lib -llibgslcblas
LIBS += -L$$PWD\lib -llibgslcblas.dll

LIBS += -L$$PWD\lib -llibfftw3-3

