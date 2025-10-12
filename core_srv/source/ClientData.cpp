#include "../include/ClientData.hpp"

ClientData::ClientData() : srv_index(-1) , keep_alive(false), reqst_is_done(false) , length(-1) , post_body_done(false) , 
	ftime_pars(false) , ftime_resp(false), resp_length(0), header_length(-1)
{

}

ClientData::ClientData(const ClientData &obj)
{
	*this = obj;
}
ClientData &ClientData::operator=(const ClientData &obj)
{
	this->srv_index = obj.srv_index;
	this->request = obj.request;
	this->response = obj.response;
	this->keep_alive = obj.keep_alive;
	this->reqst_is_done = obj.reqst_is_done;
	this->length = obj.length;
	this->post_body_done = obj.post_body_done;
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
void ClientData::set_request(std::string reqs)
{
	this->request.clear();
	this->request.insert(this->request.end(), reqs.begin(), reqs.end());
}
void ClientData::set_response(std::vector<char> resp)
{
	this->response = resp;
}
void ClientData::set_keep_alive(bool kp_alive)
{
	this->keep_alive = kp_alive;
}
void ClientData::set_reqs_done(bool reqst)
{
	this->reqst_is_done = reqst;
}
void ClientData::set_length(int new_length)
{
	this->length = new_length;
}
void ClientData::set_post_boyd(bool body)
{
	this->post_body_done = body;
}
void ClientData::set_ftime_pars(bool first_time)
{
	this->ftime_pars = first_time;
}
void 	ClientData::set_ftime_resp(bool first_time)
{
	this->ftime_resp = first_time;
}
void 	ClientData::set_resp_length(long long length)
{
	this->resp_length += length;
}
void 	ClientData::set_header_length(long long length)
{
	this->header_length = length;
}
//             Getters  
int ClientData::get_srv_index() const
{
	return (srv_index);
}
std::vector<char> &ClientData::get_request()
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
bool ClientData::get_reqs_done() const
{
	return (reqst_is_done);
}
Httprequest& ClientData::get_request_obj()
{
	return (req);
}
int ClientData::get_length()
{
	return (length);
}
bool ClientData::get_post_boolen()
{
	return (post_body_done);
}
bool ClientData::get_ftime_pars()
{
	return (ftime_pars);
}
body_data	&ClientData::get_body_struct()
{
	return (post_info);
}
std::map<std::string, std::string> &ClientData::get_body_map()
{
	return (body_content);
}
bool   		ClientData::get_ftime_resp()
{
	return (ftime_resp);
}
long long	ClientData::get_resp_length()
{
	return (resp_length);
}
long long	ClientData::get_header_length()
{
	return (header_length);
}

//              append
void    ClientData::requse_append(std::vector<char> append_req)
{
	this->request.insert(request.end(), append_req.begin(), append_req.end());
}

//              Clear
void ClientData::clean_client_data()
{
	this->request.clear();
	this->response.clear();
	this->keep_alive = false;
	this->reqst_is_done = false;
	this->length = -1;
	this->post_body_done = false;
	this->ftime_pars = false;
	this->resp_length = 0;
	this->header_length = 0;
	this->ftime_resp = false;
	this->file.clear();
}

void ClientData::clean_request()
{
	this->request.clear();
}
void ClientData::clean_response()
{
	this->response.clear();
}
