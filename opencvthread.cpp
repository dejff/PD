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
}

OpencvThread::~OpencvThread()
{
    cap.release();  //zwalnianie urządzenia, z którego był pobierany strumień wideo
}

void OpencvThread::run()
{
    QTimer frameFreezeTimer;
    connect(&frameFreezeTimer, SIGNAL(timeout()), this, SLOT(checkFreeze()), Qt::DirectConnection);
    frameFreezeTimer.start(3000);       //co 3 sekundy będzie uruchamiała się funkcja porównująca dwie ramki przechwycone w odstępnie 2-sekundowym

    exec();
}

/**
 * @brief OpencvThread::capture
 * Metoda nawiązująca połączenie z kamerą wideo i
 */
void OpencvThread::capture()
{
    Mat frame;
    cap.open(url.toUtf8().data());
    if(cap.isOpened()){
        cap.read(frame);
        ui->resolutionLabel->setText(QString::number(cap.get(CAP_PROP_FRAME_WIDTH))+"x"+QString::number(cap.get(CAP_PROP_FRAME_HEIGHT)));
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

void OpencvThread::getVideo()
{

}
