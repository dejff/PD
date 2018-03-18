#ifndef CLIENTREQUEST_H
#define CLIENTREQUEST_H

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>



class ClientRequest
{
    public:
        ClientRequest();
        virtual ~ClientRequest();
        void doRequest(string);
        void doRequest(string, string);

    protected:

    private:
};

#endif // CLIENTREQUEST_H
