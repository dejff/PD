#include "compareworker.h"

CompareWorker::CompareWorker()
{

}

CompareWorker::~CompareWorker()
{

}

void CompareWorker::compareFrames(Mat frame1, Mat frame2)
{
//    std::cout << "compare frames";
    QImage firstImage((const unsigned char *)(frame1.data), frame1.cols, frame1.rows, QImage::Format_RGB888);
    QImage secondImage((const unsigned char*)(frame2.data), frame2.cols, frame2.rows, QImage::Format_RGB888);


    double totaldiff = 0.0;
    double diffLevelVal = 0.0 ;
    if((frame1.rows == frame2.rows) && (frame1.cols == frame2.cols)){
        for ( int y = 0 ; y < firstImage.height() ; y++ ) {
            //odczytywanie linii obrazków
            uint *firstLine = ( uint* )firstImage.scanLine( y ) ;
            uint *secondLine = ( uint* )secondImage.scanLine( y ) ;
            for ( int x = 0 ; x < firstImage.width() ; x++ ) {
                uint pixelFirst = firstLine[ x ] ;
                int rFirst = qRed( pixelFirst ) ;
                int gFirst = qGreen( pixelFirst ) ;
                int bFirst = qBlue( pixelFirst ) ;

    //            //pobieranie wartości pikseli drugiego obrazka
                uint pixelSecond = secondLine[ x ] ;
    //            //pobieranie wartości składowych RGB piksela
                int rSecond = qRed( pixelSecond ) ;
                int gSecond = qGreen( pixelSecond ) ;
                int bSecond = qBlue( pixelSecond ) ;
                double rDiff = std::abs(rFirst - rSecond)/255.0;
                totaldiff+=rDiff;
                double gDiff = std::abs(gFirst - gSecond)/255.0;
                totaldiff+=gDiff;
                double bDiff = std::abs(bFirst - bSecond)/255.0;
                totaldiff+=bDiff;
            }
        }

    }
    diffLevelVal = ((totaldiff * 100)  / (firstImage.width() * firstImage.height() * 3));

    emit returnDiffVal(diffLevelVal);
}

