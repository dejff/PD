#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QThread>


class SocketThread: public QThread
{
public:
    SocketThread();
    void run() override;
    void listen();
};

#endif // SOCKETTHREAD_H
