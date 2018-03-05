#include "socketthread.h"
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

SocketThread::SocketThread()
{

}

void SocketThread::run()
{
    exec();
}

void SocketThread::listen()
{

}
