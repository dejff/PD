#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 50000

using namespace std;

int main(int argc, char* argv[])
{

    struct sockaddr_in address;
    int sock, response, portnumber, strlength;
    struct sockaddr_in server_address;
    const char *request = "Pokaż kotku co masz w środku";       //ciąg znaków wysyłany z klienta do serwera po nawiązaniu połączenia
    char bufor[1024];

    if(argc<=3 && argc>1){        //maksymalnie 3 parametry
        sock = socket(AF_INET, SOCK_STREAM, 0);     //próba zaincjalizowania socketu dla prptokołu IPv4, pakiety TCP
        if(sock<0){
            cout<<"Błąd próby zaincjalozowania socketu"<<endl;
            return -1;
        }

        memset(&server_address, '0', sizeof(server_address));     //wyzerowanie pamięci przechowujacej adres
        server_address.sin_family = AF_INET;

        if(3==argc){        //jeśli są dwa parametry to drugi jest portem
            portnumber = atoi(argv[2]);
        }else{
            portnumber = PORT;         //
        }

        server_address.sin_port = htons(portnumber);

        if(inet_pton(AF_INET, argv[1], &server_address.sin_addr)<=0){
            cout<<"Adres niepoprawny"<<endl;
            return -1;
        }

        if(connect(sock, (struct sockaddr *)&server_address, sizeof(server_address))<0){
            cout<<"Błąd połączenia"<<endl;
            return -1;
        }

        strlength = write(sock, request, strlen(request));      //funkcja odczytująca bajty z requesta i zapisująca je socket'cie, zwraca ilość zapisanych bajtów

        if(strlength<0) cout<<"Błąd zapisu do socketa";         //nie zapiasano żadnych danych - błąd

        strlength = read(sock, bufor, 1023);      //funkcja odczytująca dane z socketa i zapisująca je w zmiennej response

        if(strlength<0) cout<<"Błąd odczytu z socketa";             //nie odczytano żadnych danych - błąd

        printf("%s\n", bufor);
//        cout<<bufor;

//        send(sock , request , strlen(request) , 0 );
//        cout<<"Hello message sent\n"<<endl;
//        response = read( sock , bufor, 1024);       //odczytywanie danych z socketu i zapisywanie do bufora
        printf("%s\n",bufor );

    }else{
        cout<<"Zła ilość argumentów"<<endl;
        cout<<"Ilość argumentów jest zła.\n"<<argc<<endl;
        cout<<"Uruchamiać program z następującymi parametrami\n"<<endl;
        cout<<"./ClientPing <adres_ip> <port>\n"<<endl;
        cout<<"<adres_ip> - adres ip, urządzenia z oprogramowaniem nadzorującym\n"<<endl;
        cout<<"<port>(opcjonalne) - port na który będzie wysyłane zapytanie, domyślnie 58"<<endl;
    }

    return 0;
}
