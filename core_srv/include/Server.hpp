#pragma once

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>

#include "../../config/config.hpp"

class Server
{
    private:
        int connection;
        std::vector<pollfd> fds;
        config &myconfig;
    public:
        //          for socket fun               //        for sockaddr_in
        Server(config &config);
        void server_start();
            void bind_socket();
            void listen_socket();
            void start_connection();
}; 