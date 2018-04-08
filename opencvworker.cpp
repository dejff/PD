#include "opencvworker.h"
#include <QDebug>

using namespace cv;

OpencvWorker::OpencvWorker()
{
    isStopPushed=false;
    frameTimer = new QTimer(this);
}

OpencvWorker::~OpencvWorker()
{

}

/**
 * @brief OpencvThread::capture
 * Metoda nawiązująca połączenie z kamerą wideo, przesyłająca strumień obrazu z niej i wyświetlająca go w labelu
 */
void OpencvWorker::capture(const QString url)
{
    qDebug()<<"Start capture";
    cap.open(url.toUtf8().data());
    if(!cap.isOpened()){
        qDebug()<<"nie działa, cap zamknięty";
        emit openCvReturnMsg("Nie można połączyć się z kamerą, sprawdź dane potrzebne do połączenia");
    }

    connect(frameTimer, SIGNAL(timeout()), this, SLOT(tick()), Qt::DirectConnection);
    frameTimer->start(30);
}

void OpencvWorker::tick()
{
    cap.read(frame);
    emit returnFrame(frame);
}

/**
 * @brief OpencvThread::stopCapture
 * Funkcja
 */
void OpencvWorker::stopCapture()
{
    frameTimer->stop();
    cap.release();
    qDebug()<<"opencv zatrzymany";
    emit capStopped();
}
