#ifndef COMPAREWORKER_H
#define COMPAREWORKER_H

#include <QObject>
#include <opencv2/opencv.hpp>

using namespace cv;

class CompareWorker : public QObject
{
    Q_OBJECT
public:
    CompareWorker();
    ~CompareWorker();

private:
    Mat compareFrame1, compareFrame2;
    void compareFrames(Mat frame1, Mat frame2);
    int diffValue;

signals:
    void returnDiffVal(const QString diffval);

public slots:
    void gotFrames(Mat comp);
};

#endif // COMPAREWORKER_H
