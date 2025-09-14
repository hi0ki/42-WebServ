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
        bool reqst_is_done;
        // time;
        /*
        
        This needs a Client class that stores things like:
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
        void set_reqs_done(bool reqst);
        //      Getters
        int get_srv_index() const;
        std::vector<char> get_request() const;
        std::vector<char> get_response() const;
        bool get_keep_alive() const;
        bool get_reqs_done() const;
        //      clear
        void clear();
        void clean_request();
        void clean_response();
};
