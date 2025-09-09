#include "../include/ClientData.hpp"

ClientData::ClientData() : fd(-1) ,srv_index(-1) {}

ClientData::ClientData(const ClientData &obj)
{
    *this = obj;
}
ClientData &ClientData::operator=(const ClientData &obj)
{
    this->fd = obj.fd;
    this->srv_index = obj.srv_index;
    this->request = obj.request;
    this->response = obj.response;

    return (*this);
}

ClientData::~ClientData() {}

//             Setters
void ClientData::set_fd(int client_fd)
{
    this->fd = client_fd;
}
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

//             Getters  
int ClientData::get_fd() const
{
    return (fd);
}
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
