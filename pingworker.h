#ifndef PINGTHREAD_H
#define PINGTHREAD_H
#include <QThread>
#include <QTimer>
//#include <pcap.h>
#include <netinet/ip_icmp.h>
#include <oping.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "errorenums.h"

#define PING_TIMEOUT 2.0
#define NO_OF_SAMPLES 10

using namespace std;

class PingWorker: public QObject
{

    Q_OBJECT

public:
    PingWorker();
    ~PingWorker();
private:
    vector<double> jitters;     //tablica przechowująca 10 pomiarów jittera
    vector<double> latencies;   //tablica przechowująca 10 pomiarów czasu odpowiedzi
    vector<double>::iterator latIter;
    vector<double>::iterator jittIter;
    pingobj_t *ping;
    bool connectionLost;
    pingobj_iter_t *iter;
    double latency, diff;     //zmienna przechowująca informacje o opóźnieniu
    long unsigned int len;
    char *ipAddr;
    QTimer *timer;
    QString message;
public slots:
    void sniff(const QString ip);
    void stopPing();
    void doPing();
    static ErrorEnums checkConnection(QString ip);

signals:
    //SYGNAŁ ZWRACAJĄCY WIADOMOŚĆ O STANIE WĄTKU, MA DWIE WARTOŚĆI "OK" - KIEDY WSZYSTKO DZIAŁA, ORAZ "BRAK POŁĄCZENIA"
    void pingReturnMessage(const ErrorEnums &);
    void returnNetworkDetails();
    void sendParams(const double &, const double &);
};

#endif // PINGTHREAD_H
