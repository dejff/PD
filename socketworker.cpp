#include "socketworker.h"

SocketWorker::SocketWorker()
{
    
}

SocketWorker::~SocketWorker()
{
    
}

void SocketWorker::waitForRequest(const QString port)
{
    server = new QTcpServer(this);

    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    if(!server->listen(QHostAddress::Any, port.toUInt()))
    {
        qDebug()<<"Serwer nie został uruchomiony";
    }
    else
    {
        qDebug()<<"Serwer uruchomiony";
    }
}


void SocketWorker::newConnection()
{
    emit requestParams();
    // przechwytujemy socketa
    response = "Brak danych";
    QTcpSocket *socket = server->nextPendingConnection();

    socket->write(response.toUtf8());
    socket->flush();

    socket->waitForBytesWritten(3000);

    socket->close();
}

void SocketWorker::gotCurrentParams(const QString response)
{
    qDebug()<<response;
    this->response = response;
}

void SocketWorker::closeSocket()
{
    server->close();
    qDebug()<<"Zamykanie socketa";
}
