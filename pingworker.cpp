#include "pingworker.h"

PingWorker::PingWorker(){

}

PingWorker::~PingWorker(){

}

/**
 * @brief PingThread::ping
 * Metoda pingująca adres ip, wprowadzony w GUI
 */
void PingWorker::sniff(const QString ip)
{
    bool connectionLost = false;
    static double timeout = PING_TIMEOUT;
    ping = ping_construct();
    if(ping_host_add(ping, ip.toUtf8().data())<0)
    {
        cout<<"Błąd dodawania adresu ip";
        cout<<"Błąd dodawania adresu IP - ping_host_add()\n"<<endl;
    }

    if(ping_setopt(ping, PING_OPT_TIMEOUT, (void*)(&timeout)))
    {
        printf("Problem z konfiguracją opcji - ping_setopt");
    }

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(doPing()), Qt::DirectConnection);
    timer->start(3000);     //ping co 3 sekundy, musi tak by

}

/**
 * @brief PingThread::stopPing
 * Metoda uruchamiana podczas kończenia wątku
 */
void PingWorker::stopPing()
{
//    pingReturnMessage(ErrorEnums::NO_ERROR);
    if(timer->isActive()) timer->stop();
    if(!latencies.empty()) latencies.clear();
    if(!jitters.empty()) jitters.clear();
    if(!connectionLost) ping_destroy(ping);
}

void PingWorker::doPing()
{
    std::cout<<"Ping"<<std::endl;
    double sumLathency = 0, avgLathency = 0, sumJitter = 0, avgJitter = 0;
    if(ping_send(ping)<0)
    {
        std::cout<<"Zbyt długi ping"<<std::endl;
        emit pingReturnMessage(ErrorEnums::CONNECTION_ERROR);
    }

    if(ping_send(ping)==0)      //nie otrzymano żadnej odpowiedzi na pinga - brak łączności
    {
        timer->stop();
        if(!latencies.empty()) latencies.clear();
        if(!jitters.empty()) jitters.clear();
        ping_destroy(ping);
        connectionLost = true;
        emit pingReturnMessage(ErrorEnums::CONNECTION_ERROR);
    }

    for(iter=ping_iterator_get(ping); iter!=NULL; iter=ping_iterator_next(iter))
    {
        len = 100;
        ping_iterator_get_info(iter, PING_INFO_LATENCY, &latency, &len);

        //jeśli tablica z opóźnieniami jest pełna najpierw usuń ostatni element
        if(latencies.size()==NO_OF_SAMPLES)
        {
            latencies.pop_back();
        }

        //utworzenie obiektu iteratora dla tablicy latencies
        latIter = latencies.begin();
        latencies.insert(latIter, latency);

        //jeśli mamy co najmniej dwa elementy w tablicy latencies to można obliczyć jitter
        if(latencies.size()>=2)
        {
            diff=latencies[0]-latencies[1];

            //jeśli tablica z wartościami jittera jest pełna najpierw usuń ostatni element
            if(jitters.size()==NO_OF_SAMPLES)
            {
                jitters.pop_back();
            }
            jittIter = jitters.begin();
            jitters.insert(jittIter, abs(latencies[0]-latencies[1]));

        }

    }

    for(jittIter = jitters.begin(); jittIter!=jitters.end(); ++jittIter)
    {
        sumJitter+=*jittIter;
    }
    for(latIter = latencies.begin(); latIter!=latencies.end(); ++latIter)
    {
        sumLathency+=*latIter;
    }

    avgLathency=sumLathency/latencies.size();
    avgJitter=sumJitter/jitters.size();
    
    emit sendParams(avgLathency, avgJitter);
}

ErrorEnums PingWorker::checkConnection(QString ip)
{
    static double timeout = PING_TIMEOUT;
    static pingobj_t *checkPing;
    checkPing = ping_construct();

    if(ping_host_add(checkPing, ip.toUtf8().data())<0)
    {
        cout<<"Błąd dodawania adresu IP - ping_host_add()"<<endl;
        return ErrorEnums::CONNECTION_ERROR;
    }

    if(ping_setopt(checkPing, PING_OPT_TIMEOUT, (void*)(&timeout)))
    {
        printf("Problem z konfiguracją opcji - ping_setopt");
        return ErrorEnums::CONNECTION_ERROR;
    }

    if(ping_send(checkPing)<0)
    {
        cout<<"Błąd wysyłania pinga: ";
        return ErrorEnums::CONNECTION_ERROR;
    }

    if(ping_send(checkPing)==0)      //nie otrzymano żadnej odpowiedzi na pinga - brak łączności
    {
        ping_destroy(checkPing);
        return ErrorEnums::CONNECTION_ERROR;
    }else {
        return ErrorEnums::NO_ERROR;
    }
}
