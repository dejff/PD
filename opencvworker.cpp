#include "opencvworker.h"
#include <QDebug>

using namespace cv;
using namespace std;

OpencvWorker::OpencvWorker()
{
    isStopPushed=false;
    frameTimer = new QTimer(this);
}

OpencvWorker::~OpencvWorker()
{

}

//void OpencvThread::run()
//{
//    Mat frame;
//    QTimer frameTimer;
//    cap.open(url.toUtf8().data());
//    cout<<"url: "<<url.toUtf8().data();
//    if(cap.isOpened()){
//        connect(&frameTimer, SIGNAL(timeout()), this, SLOT(capture()), Qt::DirectConnection);
//        frameTimer.start(30);
//    }else{
//        cap.release();
//        frameTimer.stop();
//        qDebug()<<"cap nie otawrty";
//        QErrorMessage msg;
//        msg.showMessage("Błąd połączenia wideo, proszę aprawdzić poświadczenia");
//        QThread::quit();
//    }

//    if(isStopPushed)
//    {
//        frameTimer.stop();
//        cap.release();
////        QThread::quit();
////        if(!frameTimer.isActive()){
////            ui->videoLabel->setText("Brak sygnału wideo");
////        }
//    }
//    exec();
//}

/**
 * @brief OpencvThread::capture
 * Metoda nawiązująca połączenie z kamerą wideo, przesyłająca strumień obrazu z niej i wyświetlająca go w labelu
 */
void OpencvWorker::capture(const QString url)
{
    qDebug()<<"Start capture";
    mutex.lock();
    mutex.unlock();
    cap.open(url.toUtf8().data());
    if(!cap.isOpened()){
        qDebug()<<"nie działa, cap zamknięty";
        emit openCvReturnMsg("Nie można połączyć się z kamerą");
////        ui->videoLabel->setScaledContents(true);
////        ui->videoLabel->setPixmap(QPixmap::fromImage(img));
////        ui->videoLabel->resize(ui->videoLabel->pixmap()->size());
////        ui->resolutionLabel->setText(QString::number(cap.get(CAP_PROP_FRAME_WIDTH))+"x"+QString::number(cap.get(CAP_PROP_FRAME_HEIGHT)));
    }

    connect(frameTimer, SIGNAL(timeout()), this, SLOT(tick()), Qt::DirectConnection);
    frameTimer->start(30);

//    mutex.lock();
//    while(true){   //ten while chrzani wszystko... :/ trzeba zatrzymać tą pętlę
//        cap.read(frame);
////        QMutexLocker locker(&mutex);
//        mutex.lock();
//        qDebug()<<isStopPushed;
//        if(isStopPushed) break;
//        mutex.unlock();
//        emit returnFrame(frame);
//    }
//    mutex.unlock();
}

void OpencvWorker::tick()
{
    cap.read(frame);
    emit returnFrame(frame);
    qDebug()<<isStopPushed;
    qDebug()<<"Jedziemy z kurami!!!";
}

/**
 * @brief OpencvThread::stopCapture
 * Funkcja
 */
void OpencvWorker::stopCapture()
{
    qDebug()<<"Zatrzumuję opencv";
    frameTimer->stop();
    cap.release();
    qDebug()<<"opencv zatrzymany";
//    ui->videoLabel->setPixmap(QPixmap::fromImage());
//    ui->resolutionLabel->setText("");
}
