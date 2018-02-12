#include "opencvthread.h"
#include <QDebug>

using namespace cv;

OpencvThread::OpencvThread(QString ip, Ui::MainWindow *ui)
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

/**
 * @brief OpencvThread::capture
 * Metoda nawiązująca połączenie z kamerą wideo i
 */
void OpencvThread::capture()
{
    Mat frame;
    String ip = "rtsp://admin:admin@"+ip+":554";
//    QByteArray byteArray = ip.toUtf8();
//    cap.open("rtsp://admin:admin@192.168.8.10:554");
//    cap.open("rtsp://admin:admin@"+ip+":554");
    cap.open(0);
    if(cap.isOpened()){
        cap.read(frame);
        ui->resolutionLabel->setText(QString::number(cap.get(CAP_PROP_FRAME_WIDTH))+"x"+QString::number(cap.get(CAP_PROP_FRAME_HEIGHT)));
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
int OpencvThread::checkFreeze()
{
    Mat frame1, frame2, outFrame;
    if(frame1.empty()){
        cap.read(frame1);    }
    else{
        cap.read(frame2);
    }
    if(frame1.rows==frame2.rows && frame1.cols==frame2.cols)      //jeśli obie ramki mają tyle samo weirszy i kolumn
    {
        absdiff(frame1, frame2, outFrame);
        if(countNonZero(outFrame)==0){      //ilość nie zerowych elementów w macierzy jest równa zero,
                                            //czyli obie macierze były identyczne(obie ramki wideo są takie same) zamrożenie obrazu
            QThread::quit();                //kończy się wątek
        }
        return 1;
    }
    else
    {
        return 2;
    }
}
