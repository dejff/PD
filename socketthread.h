#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QThread>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "ui_mainwindow.h"


class SocketThread: public QThread
{
    Q_OBJECT
public:
    SocketThread(Ui::MainWindow *ui);
    void run() override;
    void listen();
private:
    Ui::MainWindow *ui;
    int socket_descriptor;
    struct sockaddr_in server;
};

#endif // SOCKETTHREAD_H
