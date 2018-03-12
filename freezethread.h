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
    void run() override;
    void stopFreeze();

private:
    bool isStopped;

private slots:
    void checkFreeze();
};

#endif // FREEZETHREAD_H
