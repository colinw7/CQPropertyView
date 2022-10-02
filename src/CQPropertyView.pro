TEMPLATE = lib

TARGET = CQPropertyView

QT += widgets svg

DEPENDPATH += .

QMAKE_CXXFLAGS += -std=c++17

MOC_DIR = .moc

CONFIG += staticlib

SOURCES += \
CQPropertyView.cpp \
CQPropertyViewDelegate.cpp \
CQPropertyViewEditor.cpp \
CQPropertyViewFilter.cpp \
CQPropertyViewItem.cpp \
CQPropertyViewModel.cpp \
CQPropertyViewTree.cpp \
\
CQPropertyViewAlignType.cpp \
CQPropertyViewColorType.cpp \
CQPropertyViewFontType.cpp \
CQPropertyViewIntegerType.cpp \
CQPropertyViewPointFType.cpp \
CQPropertyViewRealType.cpp \
CQPropertyViewRectFType.cpp \
CQPropertyViewSizeFType.cpp \
CQPropertyViewType.cpp \

HEADERS += \
../include/CQPropertyView.h \
../include/CQPropertyViewDelegate.h \
../include/CQPropertyViewEditor.h \
../include/CQPropertyViewFilter.h \
../include/CQPropertyViewItem.h \
../include/CQPropertyViewModel.h \
../include/CQPropertyViewTree.h \
\
../include/CQPropertyViewAlignType.h \
../include/CQPropertyViewAngleType.h \
../include/CQPropertyViewColorType.h \
../include/CQPropertyViewFontType.h \
../include/CQPropertyViewIntegerType.h \
../include/CQPropertyViewLineDashType.h \
../include/CQPropertyViewPaletteType.h \
../include/CQPropertyViewPointFType.h \
../include/CQPropertyViewRealType.h \
../include/CQPropertyViewRectFType.h \
../include/CQPropertyViewSizeFType.h \
../include/CQPropertyViewType.h \

DESTDIR     = ../lib
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
. \
../include \
../../CQUtil/include \
../../CConfig/include \
../../CFont/include \
../../CImageLib/include \
../../CMath/include \
../../CFile/include \
../../CFileUtil/include \
../../CStrUtil/include \
../../CRegExp/include \
../../CGlob/include \
../../CUtil/include \
../../COS/include \
