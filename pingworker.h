#ifndef PINGTHREAD_H
#define PINGTHREAD_H
#include <QThread>
#include <QTimer>
#include <pcap.h>
#include <netinet/ip_icmp.h>

class PingThread: public QObject
{

    Q_OBJECT

public:
    PingThread();
//    PingThread(QString ip);
//    void run() override;
    ~PingThread();
private:
    QTimer timer;
    QString ip;
    pcap_t *handle;
    QString message;
    static void got_ping(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
public slots:
    void sniff(const QString ip);
    void stopPing();

signals:
    //SYGNAŁ ZWRACAJĄCY WIADOMOŚĆ O STANIE WĄTKU, MA DWIE WARTOŚĆI "OK" - KIEDY WSZYSTKO DZIAŁA, ORAZ "BRAK POŁĄCZENIA"
    void returnMessage(const QString &);
};

#endif // PINGTHREAD_H
