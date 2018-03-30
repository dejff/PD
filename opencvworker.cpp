#include "opencvworker.h"
#include <QDebug>
#include <stdio.h>

using namespace cv;
using namespace std;

OpencvWorker::OpencvWorker()
{

}

//OpencvThread::OpencvThread(QString url, Ui::MainWindow *ui)
//{
//    this->url = url;
//    this->ui = ui;
//    isStopPushed=false;
//}

OpencvWorker::~OpencvWorker()
{
    qDebug()<<"destruktor opencv";
    cap.release();  //zwalnianie urządzenia, z którego był pobierany strumień wideo
    qDebug()<<"Zamykam wątek";
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
    isStopPushed= false;
    qDebug()<<"URL: "+url;
    cap.open(url.toUtf8().data());
    if(cap.isOpened()){
        while(!isStopPushed){
            cap.read(frame);
            emit returnFrame(frame);
        }

//        ui->videoLabel->setScaledContents(true);
//        ui->videoLabel->setPixmap(QPixmap::fromImage(img));
//        ui->videoLabel->resize(ui->videoLabel->pixmap()->size());
//        ui->resolutionLabel->setText(QString::number(cap.get(CAP_PROP_FRAME_WIDTH))+"x"+QString::number(cap.get(CAP_PROP_FRAME_HEIGHT)));
    }else{
        emit openCvReturnMsg("Nie można połączyć się z kamerą");
        qDebug()<<"nie działa, cap zamknięty";
    }
}

/**
 * @brief OpencvThread::stopCapture
 * Funkcja
 */
void OpencvWorker::stopCapture()
{
//   qDebug()<<img_path;
    qDebug()<<"Zatrzumuję opencv";
//    cap.release();
//    frameFreezeTimer.stop();
    isStopPushed=true;
    qDebug()<<"opencv zatrzymany";
//    ui->videoLabel->setPixmap(QPixmap::fromImage());
    ui->resolutionLabel->setText("");
}
