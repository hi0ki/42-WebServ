#pragma once

#include <iostream>
#include <vector>


class ClientData
{
    private:
        int srv_index;
        std::vector<char> request;
        std::vector<char> response;
        bool keep_alive;
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
        void set_srv_index(int index);
        void set_request(std::vector<char> reqs);
        void set_response(std::vector<char> resp);
        void set_keep_alive(bool kp_alive);
        //      Getters
        int get_srv_index() const;
        std::vector<char> get_request() const;
        std::vector<char> get_response() const;
        bool get_keep_alive() const;
        //      clear
        void clear();
};
