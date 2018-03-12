#ifndef FREEZETHREAD_H
#define FREEZETHREAD_H

#include <QThread>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include "ui_mainwindow.h"

using namespace cv;

class FreezeThread: public QThread
{
    Q_OBJECT
public:
    FreezeThread();
    ~FreezeThread();
    void run() override;
    void stopFreeze();

private:
    VideoCapture cap;
    bool isStopped;

private slots:
    int checkFreeze();
};

#endif // FREEZETHREAD_H
