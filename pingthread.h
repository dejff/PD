#ifndef PINGTHREAD_H
#define PINGTHREAD_H
#include <QThread>
#include <QTimer>
#include <pcap.h>
#include <netinet/ip_icmp.h>

class PingThread: public QThread
{

    Q_OBJECT

public:
    PingThread(QString ip);
    void run() override;
    ~PingThread();

private:
    QTimer timer;
    QString ip;
    static void got_ping(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
private slots:
    void sniff();

};

#endif // PINGTHREAD_H
