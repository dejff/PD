#include "opencvthread.h"
#include <QDebug>

using namespace cv;

OpencvThread::OpencvThread()
{

}

OpencvThread::OpencvThread(QString url, Ui::MainWindow *ui)
{
    this->url = url;
    this->ui = ui;
    qDebug()<<"Rozpoczynam działanie";
    isStopPushed=false;
//    Mat connectionErrorImg = imread("../no_video.jpg", IMREAD_COLOR);
//    ui->videoLabel->setScaledContents(true);
//    ui->videoLabel->setPixmap(QPixmap::fromImage(connectionErrorImg));
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
    connect(&frameTimer, SIGNAL(timeout()), this, SLOT(capture()), Qt::DirectConnection);
    frameTimer.start(30);

    if(isStopPushed)
    {
        frameTimer.stop();
        cap.release();
//        QThread::quit();
//        if(!frameTimer.isActive()){
//            ui->videoLabel->setText("Brak sygnału wideo");
//        }
    }
    qDebug()<<"przed exec";
    exec();
    qDebug()<<"po exec";
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
