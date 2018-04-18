#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H


#include <QTcpServer>
#include <QTcpSocket>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "ui_mainwindow.h"
#include <QDebug>



class SocketWorker: public QObject
{
    Q_OBJECT
public:
    SocketWorker();
    ~SocketWorker();
private:
    QTcpServer *server;
    QTcpSocket *socket;
    QString response;
    
public slots:
    void waitForRequest(const QString port);
    void gotCurrentParams(const QString response);
    void closeSocket();
    void newConnection();
    
signals:
    void requestParams();
        
};

#endif // SOCKETTHREAD_H
