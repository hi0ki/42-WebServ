#pragma once

#include <iostream>
#include <vector>
#include <map>


#include "../../request/request.hpp"
#include "Server.hpp"

typedef struct s_body_data
{
	std::string key;
	std::string file_name;
	std::fstream file;
	bool imgs_is_done;
} body_data;

class ClientData
{
	private:
		Httprequest req;
		int srv_index;
		std::vector<char> request;
		std::vector<char> response;
		std::map<std::string, std::string> body_content;
		bool keep_alive;
		bool reqst_is_done;

		int length;
		bool post_body_done;
		bool ftime_pars;
		body_data post_info;
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
		void 				set_srv_index(int index);
		void 				set_request(std::vector<char> reqs);
		void				set_request(std::string reqs);
		void 				set_response(std::vector<char> resp);
		void 				set_keep_alive(bool kp_alive);
		void 				set_reqs_done(bool reqst);
		void 				set_length(int new_length);
		void 				set_post_boyd(bool body);
		void 				set_ftime_pars(bool first_time);
		//      Getters
		Httprequest			&get_obj_req()
		{
			return (req);
		}
		int 				get_srv_index() const;
		std::vector<char> 	&get_request();
		std::vector<char> 	get_response() const;
		bool 				get_keep_alive() const;
		bool				get_reqs_done() const;
		Httprequest&		get_request_obj();
		int    				get_length();
		bool   				get_post_boolen();
		bool   				get_ftime_pars();
		body_data   		&get_body_struct();
		std::map<std::string, std::string> &get_body_content();
		//		append
		void 				requse_append(std::vector<char> append_req);
		//      clear
		void 				clean_client_data();
		void 				clean_request();
		void 				clean_response();
};
