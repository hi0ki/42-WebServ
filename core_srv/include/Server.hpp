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

#define TIME_OUT 60


class Server
{
    private:
        int connection;
        std::vector<pollfd> fds;
        std::map<int, ClientData> clients;
        config &myconfig;
        std::map<std::string, std::map<std::string, std::string> > sessions;
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
        //          getter
        std::map<std::string, std::string> &get_session(const std::string &id)
        {
            return sessions[id];
        }
        void set_session_data(const std::string &id, const std::map<std::string, std::string> &data)
        {
            std::map<std::string, std::string> &session = sessions[id];

            for (std::map<std::string, std::string>::const_iterator it = data.begin(); it != data.end(); ++it)
            {
                session[it->first] = it->second;
            }
        }


};

void post_algo(ClientData &client, int &request_length);