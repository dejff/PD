#ifndef OPENCVTHREAD_H
#define OPENCVTHREAD_H
#include <QThread>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include "ui_mainwindow.h"

using namespace cv;

class OpencvThread: public QThread
{
public:
    OpencvThread(QString url, Ui::MainWindow *ui);
    ~OpencvThread();
    void run() override;
    void capture();
private:
    Ui::MainWindow *ui;
    QString url;
    VideoCapture cap;
    QTimer frameFreezeTimer;
    Mat frame1, frame2;
private slots:
    int checkFreeze();
};

#endif // OPENCVTHREAD_H
