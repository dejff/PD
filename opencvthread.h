#ifndef OPENCVTHREAD_H
#define OPENCVTHREAD_H
#include <QThread>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include "ui_mainwindow.h"

using namespace cv;

class OpencvThread: public QThread
{
    Q_OBJECT

public:
    OpencvThread();
    OpencvThread(QString url, Ui::MainWindow *ui);
    ~OpencvThread();
    void run() override;
    void capture();
private:
    Ui::MainWindow *ui;
    QString url;
    VideoCapture cap;
    Mat frame1, frame2;
    void getVideo();
private slots:
    int checkFreeze();
};

#endif // OPENCVTHREAD_H
