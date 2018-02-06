#include "pingthread.h"
#include <QDebug>


PingThread::PingThread(QString ip)
{
    this->ip = ip;
}

PingThread::~PingThread(){
    timer->stop();
}


void PingThread::run()
{
    timer = new QTimer(this);       //timer odmierzający czas pomiędzy kolejnymi pingami
    connect(timer, SIGNAL(timeout()),this, SLOT(ping()));     //połączenie timera z funkcją
    timer->start(5000);             //uruchomienie timera, z interwałem 5s
}

/**
 * @brief PingThread::ping
 * Metoda pingująca adres ip, wprowadzony w GUI
 */
void PingThread::ping()
{
    struct bpf_program fp;	//skompilowane wyrażenie
    bpf_u_int32 mask;		/* maska podsieci */
    bpf_u_int32 net;		/* The IP of our sniffing device */

    //wysyłanie 5 pakietów ICMP na adres podany w interfejsie graficznym
    QString ping_exp = "ping "+ip+" -c 5";
    QByteArray ping_char = ping_exp.toUtf8();

    char *dev, errbuf[PCAP_ERRBUF_SIZE];

    //otwieranie domyślnej karty sieciowej
    dev = pcap_lookupdev(errbuf);

    pcap_t *handle;
    QString filter_string = "icmp and ip src "+ip;
    const char *filter_exp = filter_string.toUtf8().data();

    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n", dev, errbuf);
//        return(1);
    }

    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
//        return(2);
    }

    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
//        return(3);
    }
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
//        return(4);
    }

    system(ping_char.data());
    //przechwytywanie pakietów
    if(pcap_dispatch(handle,5,got_ping,NULL)==0){   //pcap_dispatch zwróci 0 jeśli po timeoucie ustawionym w pcap_open_live nie zostanie przechwycony żaden pakiet
        qDebug()<<"brak połączenia z kamerą";
        quit();
    }

    pcap_close(handle);

}

void PingThread::got_ping(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    qDebug()<<"przechwycono pakiet";
}
