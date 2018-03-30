#ifndef PINGTHREAD_H
#define PINGTHREAD_H
#include <QThread>
#include <QTimer>
#include <pcap.h>
#include <netinet/ip_icmp.h>

class PingWorker: public QObject
{

    Q_OBJECT

public:
    PingWorker();
//    PingThread(QString ip);
//    void run() override;
    ~PingWorker();
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
    void pingReturnMessage(const QString &);
};

#endif // PINGTHREAD_H
