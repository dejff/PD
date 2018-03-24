#include "socketthread.h"

SocketThread::SocketThread(Ui::MainWindow *ui)
{
    this->ui = ui;
}

void SocketThread::run()
{
    waitForRequest();
//    exec();
}

void SocketThread::waitForRequest()
{
    qDebug()<<"Serwer nasłuchuje";
    char hello[] = "Siema z serwera";
    socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);		//utworzenie deskryptora gniazda
    if(socket_descriptor<0){
            qDebug()<<"Błąd tworzenia deksryptora socketu - socket";
            exit(EXIT_FAILURE);
    }


    if(setsockopt(socket_descriptor, SOL_SOCKET,
        SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
            qDebug()<<"Błąd ustawiania opcji socketu - setsocketopt";
            exit(EXIT_FAILURE);
    }

    portnumber = atoi((ui->portField->text()).toUtf8().data());

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(portnumber); //później podłączyć możliwość przesyłania własnego portu, na razie jest na sztywni 58
    qDebug()<<portnumber;
    qDebug()<<bind(socket_descriptor, (struct sockaddr *)&server, sizeof(server));
    qDebug()<<sizeof(server);

    if(bind(socket_descriptor, (struct sockaddr *)&server, sizeof(server))<0){
        qDebug()<<"Błąd bindowania socketa do portu - bind";
        exit(EXIT_FAILURE);
    }


    if(listen (socket_descriptor, 10)<0){        //maksymalna ilość użytkowników
        qDebug()<<"Błąd podczas nasłuchiwania - listen";
        exit(EXIT_FAILURE);
    }

    if((new_socket = accept(socket_descriptor, (struct sockaddr *)&server, (socklen_t*)&address_len))<0){
        qDebug()<<"Wystąpił błąd podczas nawiązywania połączenia";
        exit(EXIT_FAILURE);
    }

    valread = read(new_socket, buffer, 1024);
    qDebug()<<buffer;
    send(socket_descriptor, hello, strlen(hello), 0);


}
