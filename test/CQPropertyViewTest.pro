TEMPLATE = app

TARGET = CQPropertyViewTest

DEPENDPATH += .

QT += widgets svg

QMAKE_CXXFLAGS += -std=c++14

SOURCES += \
CQPropertyViewTest.cpp \

HEADERS += \
CQPropertyViewTest.h \

DESTDIR     = ../bin
OBJECTS_DIR = ../obj

INCLUDEPATH += \
. \
../include \
../../CQUtil/include \
../../CImageLib/include \
../../CFont/include \
../../CFile/include \
../../CMath/include \
../../CStrUtil/include \
../../CUtil/include \
../../COS/include \

unix:LIBS += \
-L../lib \
-L../../CQUtil/lib \
-L../../CImageLib/lib \
-L../../CFont/lib \
-L../../CConfig/lib \
-L../../CUtil/lib \
-L../../CFileUtil/lib \
-L../../CFile/lib \
-L../../CMath/lib \
-L../../CStrUtil/lib \
-L../../CRegExp/lib \
-L../../COS/lib \
-lCQPropertyView -lCQUtil -lCImageLib -lCFont \
-lCConfig -lCUtil -lCFileUtil -lCFile -lCMath -lCRegExp -lCStrUtil \
-lCOS -lpng -ljpeg -ltre \
