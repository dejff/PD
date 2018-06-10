#include "opencvworker.h"

using namespace cv;

OpencvWorker::OpencvWorker()
{
    isStopPushed=false;
    frameTimer = new QTimer(this);
    connect(this, SIGNAL(), compareWorker, SLOT(compareFrames(Mar frame1, Mat frame2)));
    connect()
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
//    std::cout << "tick" << std::endl;
    cap.read(frame);//odczytujemy ramkę wideo ze strumienia
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
            std::cout << "counter 100" << std::endl;
            if(compareFrame2.empty())
            {
                frame.copyTo(compareFrame2);
            }
            else
            {
//                std::cout << "w tick";
                if(!compareFrame1.empty()) compareFrame1.release();
                compareFrame2.copyTo(compareFrame1);            //skopiowanie ramki
                compareFrame2.release();
                frame.copyTo(compareFrame2);
                if(compareFrame1.rows==compareFrame2.rows && compareFrame1.cols==compareFrame2.cols)      //jeśli obie ramki mają tyle samo weirszy i kolumn
                {
                    compareFrames(compareFrame1, compareFrame2);
                }
                else
                {
//                    //Ramki mają różne wymiary - nie mogą być takie same -> BŁĄD
                    frameTimer->stop();
                    emit openCvReturnMsg(ErrorEnums::FREEZE_ERROR);
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
    cap.release();
    emit capStopped();
}

void OpencvWorker::compareFrames(Mat frame1, Mat frame2)
{

//    std::cout << "compare frames";
//    QImage firstImage((const unsigned char *)(frame1.data), frame1.cols, frame1.rows, QImage::Format_RGB888);
//    QImage secondImage((const unsigned char*)(frame2.data), frame2.cols, frame2.rows, QImage::Format_RGB888);
//    double totaldiff = 0.0 ;
//    double diffLevelVal = 0.0 ;
//    for ( int y = 0 ; y < firstImage.height() ; y++ ) {
//        //odczytywanie linii obrazków
//        uint *firstLine = ( uint* )firstImage.scanLine( y ) ;
//        uint *secondLine = ( uint* )secondImage.scanLine( y ) ;
//        for ( int x = 0 ; x < firstImage.width() ; x++ ) {
//            uint pixelFirst = firstLine[ x ] ;
//            int rFirst = qRed( pixelFirst ) ;
//            int gFirst = qGreen( pixelFirst ) ;
//            int bFirst = qBlue( pixelFirst ) ;

//            //pobieranie wartości pikseli drugiego obrazka
//            uint pixelSecond = secondLine[ x ] ;
//            //pobieranie wartości składowych RGB piksela
//            int rSecond = qRed( pixelSecond ) ;
//            int gSecond = qGreen( pixelSecond ) ;
//            int bSecond = qBlue( pixelSecond ) ;
//            int rDiff = rFirst - rSecond;
//            int gDiff = gFirst - gSecond;
//            int bDiff = bFirst - bSecond;
//            if(rDiff<0) rDiff*=(-1);
//            totaldiff += rDiff / 255.0;
//            totaldiff += qFabs( rFirst - rSecond ) / 255.0 ;
//            totaldiff += qFabs( gFirst - gSecond ) / 255.0 ;
//            totaldiff += qFabs( bFirst - bSecond ) / 255.0 ;
//        }
//    }
//    std::cout<<totaldiff<<std::endl;
//    diffLevelVal = ((totaldiff * 100)  / (firstImage.width() * firstImage.height() * 3));

//    if(diffLevelVal<DIFF_LEVEL)
//    {
//        frameTimer->stop();
//        emit openCvReturnMsg(ErrorEnums::FREEZE_ERROR);
//    }

//    emit diffLevel(QString::number(diffLevelVal));
}
