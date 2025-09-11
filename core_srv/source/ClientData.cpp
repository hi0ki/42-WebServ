#include "../include/ClientData.hpp"

ClientData::ClientData() : srv_index(-1) {}

ClientData::ClientData(const ClientData &obj)
{
    *this = obj;
}
ClientData &ClientData::operator=(const ClientData &obj)
{
    this->srv_index = obj.srv_index;
    this->request = obj.request;
    this->response = obj.response;
    this->keep_alive = false;
    return (*this);
}

ClientData::~ClientData() {}

//             Setters
void ClientData::set_srv_index(int index)
{
    this->srv_index = index;
}
void ClientData::set_request(std::vector<char> reqs)
{
    this->request = reqs;
}
void ClientData::set_response(std::vector<char> resp)
{
    this->response = resp;
}
void ClientData::set_keep_alive(bool kp_alive)
{
    this->keep_alive = kp_alive;
}

//             Getters  
int ClientData::get_srv_index() const
{
    return (srv_index);
}
std::vector<char> ClientData::get_request() const
{
    return (request);
}
std::vector<char> ClientData::get_response() const
{
    return (response);
}
bool ClientData::get_keep_alive() const
{
    return (keep_alive);
}

//              Clear
void ClientData::clear()
{
    this->srv_index = 0;
    this->request.clear();
    this->response.clear();
}
