#ifndef PINGTHREAD_H
#define PINGTHREAD_H
#include <QThread>
#include <QTimer>
#include <pcap.h>

class PingThread: public QThread
{
public:
    PingThread(QString ip);
    void run() override;
    ~PingThread();

private:
    QString ip;
    static void got_ping(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

private slots:
    void ping();

};

#endif // PINGTHREAD_H
