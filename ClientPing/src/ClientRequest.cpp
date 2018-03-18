#include "ClientRequest.h"

ClientRequest::ClientRequest()
{

}

ClientRequest::~ClientRequest()
{

}

int ClientRequest::doRequest(string ip)
{
    return ClientRequest::doRequest(ip, null);
}

int ClientRequest::doRequest(string ip, string port){
    vsock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock<0){     //próba zaincjalizowania socketu dla prptokołu IPv4, pakiety TCP
        cout<<"Błąd próby zaincjalozowania socketu"<<endl;
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(58);

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
}
