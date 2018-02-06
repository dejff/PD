#ifndef OPENCVTHREAD_H
#define OPENCVTHREAD_H
#include <QThread>
#include <opencv2/opencv.hpp>

class OpencvThread: public QThread
{
public:
    OpencvThread(QString ip);
    ~OpencvThread();
    void run() override;
private:
    QString ip;
};

#endif // OPENCVTHREAD_H
