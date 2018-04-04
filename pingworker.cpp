#include "pingworker.h"
#include <QDebug>

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
    static double timeout = PING_TIMEOUT;
    qDebug()<<"Ping działa, ip: "+ip;
//    cout<<"ip: "<<ipAddr<<endl;
    ping = ping_construct();
    if(ping_host_add(ping, ip.toUtf8().data())<0)
    {
        cout<<"Błąd dodawania adresu ip";
        cout<<"Błąd dodawania adresu IP - ping_host_add()\n"<<endl;
    }

    qDebug()<<"ipAddr";

    if(ping_setopt(ping, PING_OPT_TIMEOUT, (void*)(&timeout)))
    {
        printf("Problem z konfiguracją opcji - ping_setopt");
        emit pingReturnMessage("Brak połączenia z urządzeniem");
    }

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(doPing()), Qt::DirectConnection);
    timer->start(1000);     //ping co 1 sekundę

}

/**
 * @brief PingThread::stopPing
 * Metoda uruchamiana podczas kończenia wątku
 */
void PingWorker::stopPing()
{
    pingReturnMessage("ZATRZYMANO");
    timer->stop();
    if(!latencies.empty()) latencies.clear();
    if(!jitters.empty()) jitters.clear();
    ping_destroy(ping);
}

void PingWorker::doPing()
{
    double sumLathency = 0, avgLathency = 0, sumJitter = 0, avgJitter = 0;
    if(ping_send(ping)<0)
    {
        cout<<"Błąd wysyłania pinga: "<<ping_get_error(ping)<<endl;
    }

    if(ping_send(ping)==0)      //nie otrzymano żadnej odpowiedzi na pinga - brak łączności
    {
        cout<<"Problem z połączeniem";
    }

    for(iter=ping_iterator_get(ping); iter!=NULL; iter=ping_iterator_next(iter))
    {
        len = 100;
        ping_iterator_get_info(iter, PING_INFO_LATENCY, &latency, &len);
        cout<<"Czas odpowiedzi: "<<latency<<endl;


        if(latencies.size()==NO_OF_SAMPLES)
        {
            latencies.pop_back();
        }

        latIter = latencies.begin();
        latencies.insert(latIter, latency);

        if(latencies.size()>=2)
        {
            diff=latencies[0]-latencies[1];

            if(jitters.size()==NO_OF_SAMPLES)
            {
                jitters.pop_back();
            }
            jittIter = jitters.begin();
            jitters.insert(jittIter, abs(latencies[0]-latencies[1]));

        }

    }
    qDebug()<<jitters.size();

    for(jittIter = jitters.begin(); jittIter!=jitters.end(); ++jittIter)
    {
        qDebug()<<*jittIter;
        sumJitter+=*jittIter;
    }
    for(latIter = latencies.begin(); latIter!=latencies.end(); ++latIter)
    {
        sumLathency+=*latIter;
    }

    avgLathency=sumLathency/latencies.size();
    avgJitter=sumJitter/jitters.size();
    qDebug()<<"Suma jittera: "+QString::number(sumJitter);
    qDebug()<<"Średnia jittera: "+QString::number(avgJitter);

}
