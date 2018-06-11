#include "opencvworker.h"

using namespace cv;

OpencvWorker::OpencvWorker()
{
    compareWorker = new CompareWorker();
    compareWorker->moveToThread(&compareThread);
    isStopPushed=false;
    frameTimer = new QTimer(this);
    qRegisterMetaType<Mat>("Mat");
    connect(this, SIGNAL(compareThisFrames(Mat,Mat)), compareWorker, SLOT(compareFrames(Mat, Mat)));
    connect(compareWorker, SIGNAL(returnDiffVal(double)), this, SLOT(gotDiffVal(double)));
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
    if(!compareFrame1.empty()) compareFrame1.release();
    if(!compareFrame2.empty()) compareFrame2.release();
    cap.open(url.toUtf8().data());
    if(!cap.isOpened()){
        emit openCvReturnMsg(ErrorEnums::CONNECTION_ERROR);
    }

    connect(frameTimer, SIGNAL(timeout()), this, SLOT(tick()), Qt::DirectConnection);
    frameTimer->start(40);
}

void OpencvWorker::tick()
{
    cap.read(frame);//odczyt ramkę wideo ze strumienia
    if(!frame.empty()){ //jeśli ramka jest pusta to jej nie wysyłamy -
                        //w przypadku, kiedy zostanie zerwane połączenie i nie ma się wyświetlać puste okienko,
                        //bo program będzie działał jeszcze przez około 7 sekund
        emit returnFrame(frame);
    }

    if(compareFrame1.empty() && compareFrame2.empty())      //jeśli oba kontenery na ramki do porównania są puste, to zainicjuj jedną - zadziała tylko w przypadku pobrania pierwszej ramki
    {
        compareFrame1=frame;
    }
    else
    {
        if (counter == 100)
        {
            if(compareFrame2.empty())
            {
                frame.copyTo(compareFrame2);
            }
            else
            {
                if(!compareFrame1.empty()) compareFrame1.release();
                compareFrame2.copyTo(compareFrame1);            //skopiowanie ramki
                compareFrame2.release();
                frame.copyTo(compareFrame2);
                if(compareFrame1.rows==compareFrame2.rows && compareFrame1.cols==compareFrame2.cols)      //jeśli obie ramki mają tyle samo weirszy i kolumn
                {
                    //tutaj uruchamia się wątek CompareWorker
                    compareThisFrames(compareFrame1, compareFrame2);
                    compareThread.start();
//                    compareFrames(compareFrame1, compareFrame2);
                }
                else
                {
//                    //Ramki mają różne wymiary - nie mogą być takie same -> BŁĄD
                    frameTimer->stop();
//                    emit openCvReturnMsg(ErrorEnums::FREEZE_ERROR);
                }

            }
            counter=0;
        }
    }

    counter++;
}

/**
 * @brief OpencvThread::stopCapture
 * Funkcja
 */
void OpencvWorker::stopCapture()
{
    compareFrame1.release();
    compareFrame2.release();
    frameTimer->stop();
    compareThread.quit();
    compareThread.wait();
    cap.release();
    emit capStopped();
}

void OpencvWorker::gotDiffVal(const double diffVal)
{
    if(diffVal<DIFF_LEVEL)
    {
        emit openCvReturnMsg(ErrorEnums::FREEZE_ERROR);
        frameTimer->stop();
        compareThread.quit();
        compareThread.wait();
    }
    emit diffLevel(QString::number(diffVal));
}
