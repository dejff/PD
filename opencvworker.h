#ifndef OPENCVTHREAD_H
#define OPENCVTHREAD_H
#include <QThread>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include "ui_mainwindow.h"
//#include "mainwindow.h"
#include <QErrorMessage>
#include <QMutex>

using namespace cv;

class OpencvWorker: public QObject
{
    Q_OBJECT

public:
    OpencvWorker();
    ~OpencvWorker();
private:
    QMutex mutex;
    bool isStopPushed;
    QTimer *frameTimer;
    QImage img;
    Ui::MainWindow *ui;
    QString url;
    VideoCapture cap;
    Mat frame;
public slots:
    void capture(const QString url);
    void stopCapture();
    void tick();
signals:
    void returnFrame(const Mat frame);
    void openCvReturnMsg(const QString msg);
};

#endif // OPENCVTHREAD_H
