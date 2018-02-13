#include "pingthread.h"
#include <QDebug>


PingThread::PingThread(QString ip)
{
    this->ip = ip;
}

PingThread::~PingThread(){
    timer.stop();
}


void PingThread::run()
{
    QTimer timer;       //timer odmierzający czas pomiędzy kolejnymi pingami
    connect(&timer, SIGNAL(timeout()), this, SLOT(sniff()), Qt::DirectConnection);     //połączenie timera z funkcją
    timer.start(500);             //uruchomienie timera, z interwałem 5s
    exec();
}

/**
 * @brief PingThread::ping
 * Metoda pingująca adres ip, wprowadzony w GUI
 */
void PingThread::sniff()
{
    int pingCount=0;
    struct bpf_program fp;	//skompilowane wyrażenie
    bpf_u_int32 mask;		/* maska podsieci */
    bpf_u_int32 net;		/* The IP of our sniffing device */

    //wysyłanie 5 pakietów ICMP na adres podany w interfejsie graficznym


    char *dev, errbuf[PCAP_ERRBUF_SIZE];

    //otwieranie domyślnej karty sieciowej
    dev = pcap_lookupdev(errbuf);

    pcap_t *handle;
    QString filter_string = "icmp and ip src "+ip;
    const char *filter_exp = filter_string.toUtf8().data();

    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n", dev, errbuf);
    }

    handle = pcap_open_live(dev, BUFSIZ, 0, 500, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
    }

    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
    }
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
    }

    QString ping_exp = "ping "+ip+" -c 5 >/dev/null 2>&1";   //wykonanie pinga bez wyświetlania informacji w terminalu
    QByteArray ping_char = ping_exp.toUtf8();               //przekonwertowania zmiennej typu string na bytearray, żeby dalej można było ją przekonwertować na typ char

    system(ping_char.data());

    //przechwytywanie pakietów
    if(pcap_dispatch(handle,0,got_ping,NULL)==0){   //pcap_dispatch zwróci 0 jeśli po timeoucie ustawionym w pcap_open_live nie zostanie przechwycony żaden pakiet
        QThread::quit();                    //jeśli nie zostaną przechwycone żadne pakiety w odpowiedzi na ping, to wątek się zakończy
    }

    pcap_close(handle);

}

void PingThread::got_ping(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
//    qDebug()<<"pakiet:";
}
