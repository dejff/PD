#include "opencvthread.h"

using namespace cv;

OpencvThread::OpencvThread(QString ip)
{
    this->ip = ip;
}

OpencvThread::~OpencvThread()
{

}

void OpencvThread::run()
{
    capture();
    exec();
}

void OpencvThread::capture()
{
    VideoCapture cap;
    String ip = "rtsp://admin:admin@"+ip+":554";
//    QByteArray byteArray = ip.toUtf8();
    cap.open("rtsp://admin:admin@192.168.8.10:554");
//    cap.open("rtsp://admin:admin@"+ip+":554");
//    cap.open(0);
    if(cap.isOpened()){
        cap.get(CAP_PROP_FRAME_WIDTH);
        cap.get(CAP_PROP_FRAME_HEIGHT);
        cap.get(CAP_PROP_FOURCC);
    }
}
