#ifndef OPENCVTHREAD_H
#define OPENCVTHREAD_H
#include <QThread>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include "ui_mainwindow.h"
//#include <QMessageBox>
#include <QErrorMessage>

using namespace cv;

class OpencvWorker: public QObject
{
    Q_OBJECT

public:
    OpencvWorker();
    ~OpencvWorker();
private:
    bool isStopPushed;
    QTimer frameFreezeTimer;
    QImage img;
    Ui::MainWindow *ui;
    QString url;
    VideoCapture cap;
    Mat frame;
    void getVideo();
public slots:
    void capture(const QString url);
    void stopCapture();
signals:
    void returnFrame(const Mat frame);
    void openCvReturnMsg(const QString msg);
};

#endif // OPENCVTHREAD_H
