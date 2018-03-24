#include "opencvthread.h"
#include <QDebug>
#include <stdio.h>

using namespace cv;
using namespace std;

OpencvThread::OpencvThread()
{

}

OpencvThread::OpencvThread(QString url, Ui::MainWindow *ui)
{
    this->url = url;
    this->ui = ui;
    isStopPushed=false;
}

OpencvThread::~OpencvThread()
{
//    qDebug()<<"destruktor opencv";
//    if(cap.isOpened())
//    {
//        qDebug()<<"Zamykam wątek";
//        cap.release();  //zwalnianie urządzenia, z którego był pobierany strumień wideo
//    }
}

void OpencvThread::run()
{
    Mat frame;
    QTimer frameTimer;
    cap.open(url.toUtf8().data());
    cout<<"url: "<<url.toUtf8().data();
    if(cap.isOpened()){
        connect(&frameTimer, SIGNAL(timeout()), this, SLOT(capture()), Qt::DirectConnection);
        frameTimer.start(30);
    }else{
        cap.release();
        frameTimer.stop();
        qDebug()<<"cap nie otawrty";
        QErrorMessage msg;
        msg.showMessage("Błąd połączenia wideo, proszę aprawdzić poświadczenia");
        QThread::quit();
    }

    if(isStopPushed)
    {
        frameTimer.stop();
        cap.release();
//        QThread::quit();
//        if(!frameTimer.isActive()){
//            ui->videoLabel->setText("Brak sygnału wideo");
//        }
    }
    exec();
}

/**
 * @brief OpencvThread::capture
 * Metoda nawiązująca połączenie z kamerą wideo, przesyłająca strumień obrazu z niej i wyświetlająca go w labelu
 */
void OpencvThread::capture()
{
    if(cap.isOpened()){
        cap.read(frame);
        img = QImage((const unsigned char*)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888);
        ui->videoLabel->setScaledContents(true);
        ui->videoLabel->setPixmap(QPixmap::fromImage(img));
        ui->videoLabel->resize(ui->videoLabel->pixmap()->size());
        ui->resolutionLabel->setText(QString::number(cap.get(CAP_PROP_FRAME_WIDTH))+"x"+QString::number(cap.get(CAP_PROP_FRAME_HEIGHT)));
    }else{
        qDebug()<<"nie działa, cap zamknięty";
    }
}

/**
 * @brief OpencvThread::stopCapture
 * Funkcja
 */
void OpencvThread::stopCapture()
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
