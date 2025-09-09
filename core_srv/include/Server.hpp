#pragma once

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>

#include "../../config/config.hpp"
// #include "ServerConfig.hpp"

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"  

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
            void bind_socket(int srv_index);
            void listen_socket();
            // void start_connection();
            void start_connection();
            bool is_server(int fd);
}; 