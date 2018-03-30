#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QThread>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "ui_mainwindow.h"
#include <QDebug>

#define PORT 58

class SocketWorker: public QThread
{
    Q_OBJECT
public:
    SocketWorker(Ui::MainWindow *ui);
    void run() override;
    void waitForRequest();
private:
    int opt = 1;
    Ui::MainWindow *ui;
    int socket_descriptor, new_socket, valread, portnumber;
    char buffer[1024];
    struct sockaddr_in server;
    int address_len = sizeof(server);
};

#endif // SOCKETTHREAD_H
