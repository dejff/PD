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
    compareFrame1.release();
    compareFrame2.release();
    qDebug()<<"Start capture";
    cap.open(url.toUtf8().data());
    if(!cap.isOpened()){
        qDebug()<<"nie działa, cap zamknięty";
        emit openCvReturnMsg(ErrorEnums::CONNECTION_ERROR);
    }

    connect(frameTimer, SIGNAL(timeout()), this, SLOT(tick()), Qt::DirectConnection);
    frameTimer->start(30);
}

void OpencvWorker::tick()
{

    qDebug()<<counter;
    cap.read(frame);
    emit returnFrame(frame);

    if(compareFrame1.empty() && compareFrame2.empty())      //jeśli oba kontenery na ramki do porównania są puste, to zainicjuj jedną - zadziała tylko w przypadku pobrania pierwszej ramki
    {
        compareFrame1=frame;
    }
    else
    {
        if(counter==100)
        {
            if(compareFrame2.empty())
            {
                qDebug()<<"Ramka 2 pusta";
                frame.copyTo(compareFrame2);
            }
            else
            {
                qDebug()<<"Ramka 1 i 2 pełna";
                compareFrame1.release();
//                qDebug()<<compareFrame1.empty();
                compareFrame2.copyTo(compareFrame1);            //skopiowanie ramki
                compareFrame2.release();
                frame.copyTo(compareFrame2);
                compareFrames(compareFrame1, compareFrame2);
                bool isEqual = (sum(compareFrame1 != compareFrame2) == Scalar(0,0,0,0));
                qDebug()<<isEqual;

            }
            counter=0;
        }

//        if(!compareFrame1.empty() && !compareFrame2.empty())
//        {
////            qDebug()<<"obie ramki nie są puste";
//            if(!compareFrames(compareFrame1, compareFrame2))
//            {
////                qDebug()<<"TEST!@#";
//                frameTimer->stop();
//                emit returnFrame(frame);
//            }
//            else
//            {
////                qDebug()<<"TEst123";
//                emit openCvReturnMsg(ErrorEnums::FREEZE_ERROR);
//            }
//        }
    }

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

    QImage firstImage((const unsigned char*)(frame1.data), frame1.cols, frame1.rows, QImage::Format_RGB888);
    QImage secondImage((const unsigned char*)(frame2.data), frame2.cols, frame2.rows, QImage::Format_RGB888);



    double totaldiff = 0.0 ; //holds the number of different pixels
    int h = firstImage.height( ) ;
    int w = firstImage.width( ) ;
    int hsecond = secondImage.height( ) ;
    int wsecond = secondImage.width( ) ;
    if ( w != wsecond || h != hsecond ) {
       qDebug() << "Error, pictures must have identical dimensions!\n" ;
    }
    for ( int y = 0 ; y < h ; y++ ) {
       uint *firstLine = ( uint* )firstImage.scanLine( y ) ;
       uint *secondLine = ( uint* )secondImage.scanLine( y ) ;
       for ( int x = 0 ; x < w ; x++ ) {
      uint pixelFirst = firstLine[ x ] ;
      int rFirst = qRed( pixelFirst ) ;
      int gFirst = qGreen( pixelFirst ) ;
      int bFirst = qBlue( pixelFirst ) ;
      uint pixelSecond = secondLine[ x ] ;
      int rSecond = qRed( pixelSecond ) ;
      int gSecond = qGreen( pixelSecond ) ;
      int bSecond = qBlue( pixelSecond ) ;
      totaldiff += std::abs( rFirst - rSecond ) / 255.0 ;
      totaldiff += std::abs( gFirst - gSecond ) / 255.0 ;
      totaldiff += std::abs( bFirst -bSecond ) / 255.0 ;
       }
    }

    emit diffLevel(QString::number((totaldiff * 100)  / (w * h * 3)));

//    if(isEqual){
//        qDebug()<<"Obrazy są takie same";
//        frameTimer->stop();
//        imwrite("diff.jpg", diffFrame);
//        emit openCvReturnMsg(ErrorEnums::FREEZE_ERROR);
//    }else{
//        qDebug()<<"Obrazy są różne";
//        absdiff(frame1, frame1, diffFrame);
//        imwrite("diff.jpg", diffFrame);
//    }

//    if(frame1.rows==frame2.rows && frame1.cols==frame2.cols)      //jeśli obie ramki mają tyle samo weirszy i kolumn
//    {

//        if(countNonZero(compareFrame1!=compareFrame2)==0){      //ilość nie zerowych elementów w macierzy jest równa zero,
//                                            //czyli obie macierze były identyczne(obie ramki wideo są takie same) zamrożenie obrazu
//            qDebug()<<"ramki są takie same";
//            return true;
//////            stopCapture();                //kończy się wątek
//        }
//        qDebug()<<"Ramki są różne";

//    }
//    else
//    {
//        qDebug()<<"Ramki są różne";
//        return false;
//    }
}
