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

    counter = 0;
    qDebug()<<"Start capture";
    cap.open(url.toUtf8().data());
    if(!cap.isOpened()){
        qDebug()<<"nie działa, cap zamknięty";
        emit openCvReturnMsg(ErrorEnums::CONNECTION_ERROR);
//        emit openCvReturnMsg("Nie można połączyć się z kamerą, sprawdź dane potrzebne do połączenia");
    }

    connect(frameTimer, SIGNAL(timeout()), this, SLOT(tick()), Qt::DirectConnection);
    frameTimer->start(30);
}

void OpencvWorker::tick()
{

//    qDebug()<<counter;
    cap.read(frame);

    if(compareFrame1.empty() && compareFrame2.empty())      //jeśli oba kontenery na ramki do porównania są puste, to zainicjuj jedną - zadziała tylko w przypadku pobrania pierwszej ramki
    {
        compareFrame1=frame;
    }
    else
    {
        if(counter==100)
        {
            qDebug()<<"Ramka 1";
            qDebug()<<compareFrame1.empty();
            qDebug()<<"Ramka 2";
            qDebug()<<compareFrame2.empty();
            qDebug()<<"100 ramek";

            if(compareFrame2.empty())
            {
                qDebug()<<"Ramka 2 pusta";
                frame.copyTo(compareFrame2);
                qDebug()<<"po kopiowaniu";
//                compareFrame2 = frame;
            }
            else
            {
                qDebug()<<"Ramka 1 i 2 pełna";
                compareFrame1.release();
                qDebug()<<compareFrame1.empty();
                compareFrame2.copyTo(compareFrame1);            //skopiowanie ramki
//                Mat::copyTo(compareFrame1, compareFrame2);
//                        ()compareFrame1=compareFrame2;
                compareFrame2.release();
//                compareFrame2=frame;
                frame.copyTo(compareFrame2);

            }
            qDebug()<<"Po ogarnianiu ramek";
            counter=0;
        }

        if(!compareFrame1.empty() && !compareFrame2.empty())
        {
            qDebug()<<"obie ramki nie są puste";
            if(!compareFrames(compareFrame1, compareFrame2))
            {
                qDebug()<<"TEST!@#";
                emit returnFrame(frame);
            }
            else
            {
                qDebug()<<"TEst123";
                emit openCvReturnMsg(ErrorEnums::FREEZE_ERROR);
            }
        }
    }



//    qDebug()<<"Po porównywaniu";

    counter++;
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

bool OpencvWorker::compareFrames(Mat frame1, Mat frame2)
{
//    qDebug()<<"compare frames";
    Mat outFrame;
    if(frame1.rows==frame2.rows && frame1.cols==frame2.cols)      //jeśli obie ramki mają tyle samo weirszy i kolumn
    {
//        qDebug()<<"Obie ramki mają taki sam rozmiar";

//        subtract(compareFrame1, compareFrame2, outFrame);
//        absdiff(frame1, frame2, outFrame);
//        compare(compareFrame1, compareFrame2, outFrame, cv::CMP_NE);
//        qDebug()<<"elemenmty nie zerowe";
//        qDebug()<<countNonZero(compareFrame1!=compareFrame2);
        if(countNonZero(compareFrame1!=compareFrame2)==0){      //ilość nie zerowych elementów w macierzy jest równa zero,
                                            //czyli obie macierze były identyczne(obie ramki wideo są takie same) zamrożenie obrazu
            qDebug()<<"ramki są takie same";
            return true;
////            stopCapture();                //kończy się wątek
        }
        qDebug()<<"Ramki są różne";

    }
    else
    {
        qDebug()<<"Ramki są różne";
        return false;
    }
}
