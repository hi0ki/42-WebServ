#pragma once

#include <iostream>
#include <vector>
#include <map>

#include "../../request/Request.hpp"
#include "Server.hpp"
#include "../../CGI/HTTPCGI.hpp"

typedef struct s_body_data
{
	std::string key;
	std::string file_name;
} body_data;

class ClientData
{
	private:
		Httprequest req;
		int srv_index;
		std::vector<char> request; // Request from recv
		std::map<std::string, std::string> body_content; // form content from Post method (key = value)
		bool keep_alive;
		bool reqst_is_done; // recv doen with request
		std::map<std::string, std::string> session_data; // cookie data
		std::string session_id; // session id

		int length; // body length for post Method
		bool post_body_done; // if recv done with body (POST method)
		bool ftime_pars; // first time for POST method
		body_data post_info; // variables for POST part

		//response vars
			long long header_length;
			long long resp_length;
			bool ftime_resp;
			std::ifstream file;
			HTTPCGI cgi;
	public:
		ClientData();
		ClientData(const ClientData &obj);
		ClientData &operator=(const ClientData &obj);
		~ClientData();
		// Setters
		void 				set_srv_index(int index);
		void 				set_request(std::vector<char> reqs);
		void				set_request(std::string reqs);
		void 				set_keep_alive(bool kp_alive);
		void 				set_reqs_done(bool reqst);
		void 				set_length(int new_length);
		void 				set_post_boyd(bool body);
		void 				set_ftime_pars(bool first_time);
		void 				set_ftime_resp(bool first_time);
		void 				set_resp_length(long long length);
		void 				set_header_length(long long length);
		void 				set_sessionID(std::string id);
		std::map<std::string, std::string>& getSession_data();
		void setSession_data(const std::string &key, const std::string &value);
		void setSession_data(const std::map<std::string, std::string> &data);
		void set_cgi(HTTPCGI _cgi);

		//      Getters
		int 				get_srv_index() const;
		std::vector<char> 	&get_request();
		bool 				get_keep_alive() const;
		bool				get_reqs_done() const;
		Httprequest&		get_request_obj();
		int    				get_length();
		bool   				get_post_boolen();
		bool   				get_ftime_pars();
		body_data   		&get_body_struct();
		std::map<std::string, std::string> &get_body_map();
		bool   				get_ftime_resp();
		long long			get_resp_length();
		long long			get_header_length();
		std::string			get_sessionID();
		HTTPCGI				&get_cgi();
		std::ifstream &getFile();
		Httprequest			&get_obj_req();
		//		append
		void 				requse_append(std::vector<char> append_req);
		//      clear
		void 				clean_client_data();
		void 				clean_request();

};
