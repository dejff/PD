#include "compareworker.h"

CompareWorker::CompareWorker()
{

}

CompareWorker::~CompareWorker()
{

}

void CompareWorker::compareFrames(Mat frame1, Mat frame2)
{
    std::cout << "compare frames";
    QImage firstImage((const unsigned char *)(frame1.data), frame1.cols, frame1.rows, QImage::Format_RGB888);
    QImage secondImage((const unsigned char*)(frame2.data), frame2.cols, frame2.rows, QImage::Format_RGB888);
    double totaldiff = 0.0 ;
    double diffLevelVal = 0.0 ;
    for ( int y = 0 ; y < firstImage.height() ; y++ ) {
        //odczytywanie linii obrazków
        uint *firstLine = ( uint* )firstImage.scanLine( y ) ;
        uint *secondLine = ( uint* )secondImage.scanLine( y ) ;
        for ( int x = 0 ; x < firstImage.width() ; x++ ) {
            uint pixelFirst = firstLine[ x ] ;
            int rFirst = qRed( pixelFirst ) ;
            int gFirst = qGreen( pixelFirst ) ;
            int bFirst = qBlue( pixelFirst ) ;

            //pobieranie wartości pikseli drugiego obrazka
            uint pixelSecond = secondLine[ x ] ;
            //pobieranie wartości składowych RGB piksela
            int rSecond = qRed( pixelSecond ) ;
            int gSecond = qGreen( pixelSecond ) ;
            int bSecond = qBlue( pixelSecond ) ;
            int rDiff = rFirst - rSecond;
            int gDiff = gFirst - gSecond;
            int bDiff = bFirst - bSecond;
            if(rDiff<0) rDiff*=(-1);
            totaldiff += rDiff / 255.0;
            totaldiff += qFabs( rFirst - rSecond ) / 255.0 ;
            totaldiff += qFabs( gFirst - gSecond ) / 255.0 ;
            totaldiff += qFabs( bFirst - bSecond ) / 255.0 ;
        }
    }
//    std::cout<<totaldiff<<std::endl;
    diffLevelVal = ((totaldiff * 100)  / (firstImage.width() * firstImage.height() * 3));

//    if(diffLevelVal<DIFF_LEVEL)
//    {
//        frameTimer->stop();
//        emit openCvReturnMsg(ErrorEnums::FREEZE_ERROR);
//    }

    emit returnDiffVal(QString::number(diffLevelVal));
}

