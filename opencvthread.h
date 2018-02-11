#ifndef OPENCVTHREAD_H
#define OPENCVTHREAD_H
#include <QThread>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include "mainwindow.h"

using namespace cv;

class OpencvThread: public QThread
{
public:
    OpencvThread(QString ip, MainWindow ui);
    ~OpencvThread();
    void run() override;
    void capture();
private:
    QString ip;
    VideoCapture cap;
    QTimer frameFreezeTimer;
    Mat frame1, frame2;
private slots:
    int checkFreeze();
};

#endif // OPENCVTHREAD_H
