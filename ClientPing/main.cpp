#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[])
{

    struct sockaddr_in address;
    int sock = 0, response;
    struct sockaddr_in server_address;
    char *request = "Pokaż kotku co masz w środku";
    char bufor[1024]={0};
    memset(&server_address, '0', sizeof(server_address));     //wyzerowanie pamięci przechowujacej adres

    if(3<=argc){
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if(sock<0){     //próba zaincjalizowania socketu dla prptokołu IPv4, pakiety TCP
            cout<<"Błąd próby zaincjalozowania socketu"<<endl;
            return -1;
        }

        server_address.sin_family = AF_INET;

        if(2==argc){
            server_address.sin_port = htons(58);
        }else{
            server_address.sin_port = htons(argv[2]);
        }


        if(inet_pton(AF_INET, argv[1], &server_address.sin_addr)<=0){
            cout<<"Adres niepoprawny"<<endl;
            return -1;
        }

        if(connect(sock, (struct sockaddr *)&server_address, sizeof(server_address))<0){
            cout<<"Błąd połączenia"<<endl;
            return -1;
        }
        send(sock , request , strlen(request) , 0 );
        cout<<"Hello message sent\n"<<endl;
        response = read( sock , bufor, 1024);       //odczytywanie danych z socketu i zapisywanie do bufora
        printf("%s\n",bufor );

    }else{
        cout<<"Zła ilość argumentów"<<endl;
    }
    cout<<"Ilość argumentów jest zła.\n"<<argc<<endl;
    cout<<"Uruchamiać program z następującymi parametrami\n"<<endl;
    cout<<"./ClientPing <adres_ip> <port>\n"<<endl;
    cout<<"<adres_ip> - adres ip, urządzenia z oprogramowaniem nadzorującym\n"<<endl;
    cout<<"<port>(opcjonalne) - port na który będzie wysyłane zapytanie, domyślnie 58"<<endl;

    return 0;
}
