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

    qDebug() << "The difference of the two pictures is " <<
         (totaldiff * 100)  / (w * h * 3)  << " % !\n" ;

    qDebug()<<"TEst";
    emit diffLevel(QString::number((totaldiff * 100)  / (w * h * 3)));


//    Mat outFrame, diff1color, gray1, gray2, diffFrame;
//    qDebug()<<"Porówanie";
//    qDebug()<<&outFrameframe1.data;
//    cvtColor(frame1, gray1, CV_BGR2GRAY);
//    cvtColor(frame2, gray2, CV_BGR2GRAY);
//    outFrame =  frame1 != frame2;

//    std::equal(frame1.begin(), frame1.end(), frame2.begin());
//    compare(gray1, gray2, outFrame, cv::CMP_NE);
//    bool isEqual = (sum(frame1 != frame2) == Scalar(0,0,0,0));
//    cv::cvtColor(outFrame, diff1color, CV_BGRA2GRAY, 1);
//    qDebug()<<
//    absdiff(frame1, frame2, outFrame);
//    qDebug()<<outFrame.data;
//    qDebug()<<(sum(frame1 != frame2) == Scalar(0,0,0,0));
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
//    imwrite("test1.jpg", frame1);
//    imwrite("test2.jpg", frame2);

//    absdiff(frame1, frame2, outFrame);
//    qDebug()<<outFrame
//    qDebug()<<"compare frames";
//    if(frame1.rows==frame2.rows && frame1.cols==frame2.cols)      //jeśli obie ramki mają tyle samo weirszy i kolumn
//    {
////        qDebug()<<"Obie ramki mają taki sam rozmiar";

////        subtract(compareFrame1, compareFrame2, outFrame);
////        absdiff(frame1, frame2, outFrame);
////        compare(compareFrame1, compareFrame2, outFrame, cv::CMP_NE);
////        qDebug()<<"elemenmty nie zerowe";
////        qDebug()<<countNonZero(compareFrame1!=compareFrame2);
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
