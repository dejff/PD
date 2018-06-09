#ifndef COMPAREWORKER_H
#define COMPAREWORKER_H

#include <QObject>
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QtMath>

using namespace cv;

class CompareWorker : public QObject
{
    Q_OBJECT
public:
    CompareWorker();
    ~CompareWorker();

private:
    Mat compareFrame1, compareFrame2;
    int diffValue;

signals:
    void returnDiffVal(const QString diffval);

public slots:
    void compareFrames(Mat frame1, Mat frame2);
};

#endif // COMPAREWORKER_H
