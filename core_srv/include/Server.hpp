#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <fstream>
#include <fcntl.h>

#include "../../config/config.hpp"
#include "../../request/Request.hpp"
#include "ClientData.hpp"

#define RESET   "\033[0m"
#define RED     "\033[31m" 
#define GREEN   "\033[32m"  
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"


class Server
{
    private:
        int connection;
        std::vector<pollfd> fds;
        std::map<int, ClientData> clients;
        config &myconfig;
    public:
        //          for socket fun               //        for sockaddr_in
        Server(config &config);
        void server_start();
            void bind_socket(int srv_index);
            void listen_socket();
            void start_connection();
            bool is_server(int fd);
        
            void accept_client(int i);
            void handle_request(int i);
            void handle_response(int i);
        //          Utils
        void pars_post_req(int index);
    
}; 
