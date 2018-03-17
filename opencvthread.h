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
    OpencvThread(QString u1rl, Ui::MainWindow *ui);
    ~OpencvThread();
    void run() override;
    void stopCapture();
private:
    bool isStopPushed;
    QTimer frameFreezeTimer;
    QImage img;
    Ui::MainWindow *ui;
    QString url;
    VideoCapture cap;
    Mat frame;
    void getVideo();
private slots:
    void capture();
};

#endif // OPENCVTHREAD_H
