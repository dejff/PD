#-------------------------------------------------
#
# Project created by QtCreator 2018-02-06T17:35:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SystemNadzoruPolaczeniaWideo
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


DEFINES += QT_NO_VERSION_TAGGING
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG(debug, debug|release) {
    DESTDIR = build/debug
    OBJECTS_DIR = build/debug
    MOC_DIR = build/debug
    RCC_DIR = build/debug
    UI_DIR = build/debug
} else {
    DESTDIR = build/release
    OBJECTS_DIR = build/release
    MOC_DIR = build/release
    RCC_DIR = build/release
    UI_DIR = build/release
}

$$OUT_PWD= $$PWD/build/release/

copydata.commands = $(COPY_DIR) $$PWD/no_video.jpg $$PWD/build/release/
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata

Release:DESTDIR = release
Release:OBJECTS_DIR = release/.obj
Release:MOC_DIR = release/.moc
Release:RCC_DIR = release/.rcc
Release:UI_DIR = release/.ui

Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug/.obj
Debug:MOC_DIR = debug/.moc
Debug:RCC_DIR = debug/.rcc
Debug:UI_DIR = debug/.ui

INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib -loping \
     -lvlc \
     -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_videoio \
     -lavutil -lavformat -lavcodec -lz -lm -lswscale -std=c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    pingworker.cpp \
    opencvworker.cpp \
    freezeworker.cpp \
    socketworker.cpp \
    videoworker.cpp

HEADERS += \
        mainwindow.h \
    pingworker.h \
    opencvworker.h \
    freezeworker.h \
    socketworker.h \
    videoworker.h \
    errorenums.h

FORMS += \
        mainwindow.ui
