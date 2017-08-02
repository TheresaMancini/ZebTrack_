#-------------------------------------------------
#
# Project created by QtCreator 2016-12-27T18:30:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ZebTrack_Novo
TEMPLATE = app


SOURCES += main.cpp\
        zebtrack.cpp \
    labeltrack.cpp \
    processamento.cpp

HEADERS  += zebtrack.h \
    labeltrack.h \
    processamento.h

FORMS    += zebtrack.ui

INCLUDEPATH += "C:\opencv\releases\install\include"
#LIBS += D://opencv//release//lib//*.dll

LIBS += -L"C:\opencv\releases\lib"\
 -llibopencv_core300 -llibopencv_imgproc300 -llibopencv_highgui300 -llibopencv_imgcodecs300 -llibopencv_video300 -llibopencv_videoio300
