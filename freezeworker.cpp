#include "freezethread.h"

FreezeThread::FreezeThread()
{

}

FreezeThread::~FreezeThread()
{

}

void FreezeThread::run()
{

    QTimer freezeTimer;
//    connect(&freezeTimer, );

}

/**
 * @brief checkFreeze
 * Metoda zwracająca 1 w przypadku kiedy ramki sa różne (obraz nie jest zamrożony), lub 2 w przypadku kiedy wystąpiło zamrożenie obrazu
 * @return
 */
int FreezeThread::checkFreeze()
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

void FreezeThread::stopFreeze()
{

}
