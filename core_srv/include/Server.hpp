#pragma once

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>


class Server
{
    private:
        int connection;
    public:
        //          for socket fun               //        for sockaddr_in
        Server(int domain, int type, int protocol, int port , u_long ip_add);

}; 