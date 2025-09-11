#pragma once

#include <iostream>
#include <vector>


class ClientData
{
    private:
        int fd;
        int srv_index;
        std::vector<char> request;
        std::vector<char> response;
        // time;
        /*
        
        This needs a Client class that stores things like:
        
            fd,

            request buffer,

            response buffer,

            server it belongs to,

            timestamp of last activity.
        
        
        */
    public:
        ClientData();
        ClientData(const ClientData &obj);
        ClientData &operator=(const ClientData &obj);
        ~ClientData();
        // Setters
        void set_fd(int client_fd);
        void set_srv_index(int index);
        void set_request(std::vector<char> reqs);
        void set_response(std::vector<char> resp);
        //      Getters
        int get_fd() const;
        int get_srv_index() const;
        std::vector<char> get_request() const;
        std::vector<char> get_response() const;
};
