QT       += core gui
QT       += multimedia
QT       += multimediawidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    imagecropperlabel.cpp \
    imgcrop.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    imagecropperlabel.h \
    imgMananger.h \
    imgcrop.h \
    mainwindow.h \
    scanner.h \
    tesseract.h

FORMS += \
    imgcrop.ui \
    mainwindow.ui

INCLUDEPATH += E:\opencv\opencv\build\include
LIBS += E:\opencv\opencv\build\x64\vc16\lib\opencv_world490d.lib

RESOURCES += \
    icon.qrc

INCLUDEPATH += E:\qtcreator\opencv_test\opencv_test\tesseract\include
LIBS+= E:\qtcreator\opencv_test\opencv_test\tesseract\lib\tesseract50.lib


