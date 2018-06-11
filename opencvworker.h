#ifndef OPENCVTHREAD_H
#define OPENCVTHREAD_H
#include <QThread>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include <QThread>
//#include "ui_mainwindow.h"
#include <QErrorMessage>
//#include <QMutex>
#include "errorenums.h"
#include "compareworker.h"

#define DIFF_LEVEL 0.01

using namespace cv;

class OpencvWorker: public QObject
{
    Q_OBJECT

public:
    OpencvWorker();
    ~OpencvWorker();
private:
    int counter;
    QThread compareThread;
//    QMutex mutex;
    bool isStopPushed;
    QTimer *frameTimer;
    QImage img;
    CompareWorker *compareWorker;
//    Ui::MainWindow *ui;
    QString url;
    VideoCapture cap;
    Mat frame, compareFrame1, compareFrame2;
//    void compareFrames(Mat frame1, Mat frame2);
public slots:
    void capture(const QString url);
    void stopCapture();
    void tick();
    void gotDiffVal(const double diffVal);
signals:
    void compareThisFrames(const Mat frame1, const Mat frame2);
    void returnFrame(const Mat frame);
    void openCvReturnMsg(const ErrorEnums err);
    void diffLevel(const QString diff);
    void capStopped();
};

#endif // OPENCVTHREAD_H
