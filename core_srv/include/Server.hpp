#pragma once

#include <iostream>
#include <sys/socket.h>


class Server
{
    private:
        int connection; 
    public:
        //          for socket fun               //        for sockaddr_in
        Server(int domain, int type, int protocol, int port , int ip_add);

}; 