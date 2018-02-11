#include "opencvthread.h"
#include <QDebug>

using namespace cv;

OpencvThread::OpencvThread(QString ip, MainWindow ui)
{
    this->ip = ip;
    this->ui = ui;
}

OpencvThread::~OpencvThread()
{
    cap.release();  //zwalnianie urządzenia, z którego był pobierany strumień wideo
}

void OpencvThread::run()
{
    capture();
    exec();
}

void OpencvThread::capture()
{
    Mat frame;
    String ip = "rtsp://admin:admin@"+ip+":554";
//    QByteArray byteArray = ip.toUtf8();
//    cap.open("rtsp://admin:admin@192.168.8.10:554");
//    cap.open("rtsp://admin:admin@"+ip+":554");
    cap.open(0);
    qDebug()<<"otwarto cap";
    if(cap.isOpened()){
        cap.read(frame);
        QString resolutiuons = cap.get(CAP_PROP_FRAME_WIDTH)+"x"+cap.get(CAP_PROP_FRAME_HEIGHT);
//        qDebug()<<cap.get(CAP_PROP_FRAME_WIDTH);
//        qDebug()<<cap.get(CAP_PROP_FRAME_HEIGHT);
        qDebug()<<QString::number(cap.get(CAP_PROP_FOURCC));
    }else{
        QThread::quit();
    }
}


/**
 * @brief checkFreeze
 * Metoda zwracająca 1 w przypadku kiedy ramki sa różne (obraz nie jest zamrożony), lub 2 w przypadku kiedy wystąpiło zamrożenie obrazu
 * @return
 */
int checkFreeze()
{
//    if(frame1==NULL){

//    }
//    absdiff(frame1, frame2);
//    if(absdiff(frame1, frame2)!=0)      //różnica macierzy frame1 i frame2 jest różna od siebie
//    {
//        return 1;
//    }
//    else
//    {
//        return 2;
//    }
}
