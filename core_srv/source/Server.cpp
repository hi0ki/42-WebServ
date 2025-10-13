#include "../include/Server.hpp"
#include "../../config/server.hpp"

#include <unistd.h>
#include <sstream>


/*
	infos:
		(pay attention to differences between HTTP versions)
		the virtual host
*/

uint32_t ip_convert(std::string ip)
{
	unsigned int b1, b2, b3, b4;
	char dot;

	std::stringstream ss(ip);
	ss >> b1 >> dot >> b2 >> dot >> b3 >> dot >> b4;

	uint32_t ipHostOrder = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;

	return (ipHostOrder);
}

Server::Server(config &config) : myconfig(config)
{

	
	this->server_start();
	if (this->myconfig.get_servs().size() == 0)
	{
		throw std::runtime_error("No server find\n" );
		return ;
	}
	std::cout << GREEN << "---------------------------------------" << std::endl;
	std::cout << "-----------Servers listening-----------" << std::endl;
	std::cout << "---------------------------------------\n" << RESET<< std::endl;
	this->start_connection();
}

void Server::server_start()
{
	for (int i = 0; i < this->myconfig.get_servs().size(); i++)
	{
		this->connection = socket(AF_INET, SOCK_STREAM, 0); //  the listening socket.
		if (this->connection == -1)
		{
			std::cerr<< "connection socket err" << std::endl;
			continue ;
		}
		try
		{
			this->bind_socket(i);
			this->listen_socket();

			// add server fd to the container
			fcntl(this->connection, F_SETFL, O_NONBLOCK);
			pollfd pfd;
			pfd.fd = this->connection;
			pfd.events = POLLIN | POLLOUT; // give events both signs
			pfd.revents = 0;
			fds.push_back(pfd);
			
		}
		catch(const std::exception& e)
		{
			std::cerr << RED << "[ " << this->myconfig.get_servs()[i].get_IP() << ":" << 
				this->myconfig.get_servs()[i].get_port() << " ]" << " : " <<
				e.what() << RESET << std::endl;
			this->myconfig.get_servs().erase(this->myconfig.get_servs().begin() + i);
			i--;
			close(this->connection);
		}
	}
}

void Server::bind_socket(int srv_index)
{
	std::cout << "------------------------biiindiiiing----------------------\n";
	std::cout << "server index : " << srv_index << std::endl;
	std::cout << "ip : " << this->myconfig.get_servs()[srv_index].get_IP() << std::endl;
	std::cout << "port : " << this->myconfig.get_servs()[srv_index].get_port() << std::endl;
	std::cout << "connection : " << this->myconfig.get_servs()[srv_index].get_port() << std::endl;
	std::cout << std::endl;

	if (this->myconfig.get_servs()[srv_index].get_port() < 1024 || this->myconfig.get_servs()[srv_index].get_port() > 65535)
	{
		std::string err_msg = std::to_string(this->myconfig.get_servs()[srv_index].get_port()) + " : Can't use ports under 1024 or bigger then 65535, your process must have the necessary permissions\n";
		throw std::runtime_error(err_msg);
	}
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip_convert(this->myconfig.get_servs()[srv_index].get_IP())); // (host to network long).
	addr.sin_port = htons(this->myconfig.get_servs()[srv_index].get_port()); // change it to bytes with htons bcs maching dont read the decimal
	
	int opt = 1;
	setsockopt(this->connection, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (bind(this->connection, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		throw std::runtime_error("bind err");
}

void Server::listen_socket()
{
	if (listen(this->connection, SOMAXCONN) == -1)
		throw std::runtime_error("listen err");
}

bool Server::is_server(int fd)
{
	for (int i = 0; i < this->myconfig.get_servs().size(); i++)
	{
		if (fd == this->fds[i].fd)
			return true;
	}
	return false;
}

void Server::start_connection()
{
	int poll_var;
	char  buffer[5000];

	while (true)
	{
		poll_var = poll(fds.data(), fds.size(), -1);
		if (poll_var == -1)
		{
			// close all fds
			throw std::runtime_error("poll err");
		}
		for (int i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				if (is_server(fds[i].fd) == true)
					this->accept_client(i);
				else
					this->handle_request(i);
			}
			else if (fds[i].revents & POLLOUT)
				this->handle_response(i);
		}
	}
}


void Server::accept_client(int i)
{
	int client_fd;
	pollfd client_pfd;
	ClientData new_client;

	std::cout << GREEN << "\n--------------------server-------------------" << RESET << std::endl;
	client_fd = accept(fds[i].fd, NULL, NULL); // socket in ESTABLISHED state for theat specific client
	if (client_fd == -1)
	{
		std::cerr << "client from this server [" << fds[i].fd <<"] failed" << std::endl; // replace fds.[i].fd with ip from my config
		return ;
	}
	//    INIT  POLLFD
	client_pfd.fd = client_fd;
	client_pfd.events = POLLIN;
	client_pfd.revents = 0;
	fds.push_back(client_pfd);

	// INIT NEW CLIENT
	std::cout << BLUE << "--------writing clinet-----------" << RESET << std::endl;
	std::cout << "client fd = " << client_fd << std::endl;
	std::cout << "server fd = " << fds[i].fd << std::endl;
	std::cout << "server index = " << i << std::endl;
	new_client.clean_client_data();
	new_client.set_srv_index(i);
	this->clients[client_fd] = new_client;
	fcntl(client_fd, F_SETFL, O_NONBLOCK);
	std::cout << "size = " << this->clients.size() << std::endl;
	std::cout << BLUE << "----------writing end-------------" << RESET << std::endl;
	std::cout << GREEN << "---------------------------------------------" << RESET << std::endl;
}

void Server::pars_post_req(int index)
{
	size_t request_length;

	if (!this->clients[index].get_ftime_pars())
	{
		std::vector<char>::iterator find_index;

		std::cout << YELLOW << "------------------ first Time -----------------" << RESET << std::endl;
		find_index = std::search(this->clients[index].get_request().begin(), this->clients[index].get_request().end(),
			std::begin("\r\n\r\n"), std::end("\r\n\r\n") - 1);
		this->clients[index].get_request().erase(this->clients[index].get_request().begin(), find_index + 4);
		request_length = this->clients[index].get_request().size();
		this->clients[index].set_ftime_pars(true);

		std::cout << GREEN << "------------------------" << RESET <<  std::endl;
		std::cout << "size = " << request_length << std::endl;
		for (int j = 0; j < this->clients[index].get_request().size() ; j++)
			std::cout << this->clients[index].get_request()[j];
		std::cout << GREEN << "------------------------" << RESET <<  std::endl;

	}
	if (!this->clients[index].get_body_struct().key.empty() && this->clients[index].get_request().size() == this->clients[index].get_length())
	{
		std::cout << YELLOW << "---------------- Not first Time ----------------" << RESET << std::endl;
		size_t key_pos = 0;
		size_t fname_pos = 0;
		std::string boundry = this->clients[index].get_body_struct().key;
		size_t boundry_size = boundry.size();
		std::string old_request(
			this->clients[index].get_request().begin(),
			this->clients[index].get_request().end()
		);

		request_length = this->clients[index].get_request().size();
		while (true)
		{
			key_pos = old_request.find(boundry);
			if (key_pos != std::string::npos)
			{
				if (old_request[key_pos + boundry_size + 1] == '-')
				{
					old_request.erase(0, key_pos + boundry_size + 4);
					this->clients[index].set_request(old_request);
					break ;
				}
				old_request.erase(0, boundry_size + key_pos + 2);
				key_pos = old_request.find(boundry) - 6;
				fname_pos = old_request.find("filename=\"");
				if (fname_pos != std::string::npos)
				{
					std::cout << GREEN << "--- Image part ---" << RESET << std::endl;
					fname_pos += 10;
					this->clients[index].get_body_struct().file_name.clear();
					for (; old_request[fname_pos] != '"'; fname_pos++)
						this->clients[index].get_body_struct().file_name.push_back(old_request[fname_pos]);
					std::cout << "file name = \"" << this->clients[index].get_body_struct().file_name << "\"" << std::endl;
					std::ofstream myfile(this->clients[index].get_request_obj().getAbsolutePath() + "/" + this->clients[index].get_body_struct().file_name.c_str());
					if (myfile.is_open())
					{
						std::cout << "File opened" << std::endl;
						int end = old_request.find("\r\n\r\n") + 4;
						myfile.write(&old_request[end], key_pos - end);
						old_request.erase(old_request.begin(), old_request.begin() + end);
						myfile.close();
					}
					else
						std::cout << RED << "File didn't open" << RESET << std::endl;
				}
				else
				{
					std::cout << GREEN << " --- Map body data ---" << RESET << std::endl;
					fname_pos = old_request.find("name=\"");
					std::string map_key;
					std::string map_value;

					if (fname_pos != std::string::npos)
					{
						fname_pos += 6;
						for (; old_request[fname_pos] != '"'; fname_pos++)
							map_key.push_back(old_request[fname_pos]);
						int end = old_request.find("\r\n\r\n") + 4;
						for (;end < key_pos; end++)
							map_value.push_back(old_request[end]);
						old_request.erase(old_request.begin(), old_request.begin() + end);
						this->clients[index].get_body_map()[map_key] = map_value;
						
						std::cout << "map key = \"" << map_key << "\"" << std::endl;
						std::cout << "map value = " << this->clients[index].get_body_map()[map_key]<< std::endl;

						map_key.clear();
						map_value.clear();
					}
				}
			}
			else 
				break;
		}
	}
	if (request_length == this->clients[index].get_length()) // hadi khasra hit kaydkhl mn awl req katwsl  o howa khaso ikml req kamla 3ad idkhl liha
	{
		this->clients[index].set_post_boyd(true);
		this->clients[index].set_reqs_done(true);
		this->clients[index].set_length(-1);
	}
}

void Server::handle_request(int i)
{
	std::cout << YELLOW << "\n[" << fds[i].fd << "]" << " : Client Request" <<  RESET << std::endl;
	char buffer[4096];
	int bytesRead = recv(fds[i].fd, buffer, sizeof(buffer), 0);
	if (bytesRead > 0) {
		this->clients[fds[i].fd].get_request().insert(
			this->clients[fds[i].fd].get_request().end(), buffer, buffer + bytesRead
		);
		this->clients[fds[i].fd].update_activity();
	}
	else if (bytesRead == 0)
	{
		std::cerr << RED << "[" << fds[i].fd << "]" << " : Client disconnected: fd " << fds[i].fd << RESET << std::endl;
		close(fds[i].fd);
		this->clients.erase(fds[i].fd);
		this->fds.erase(fds.begin() + i);
		i--;
		return ;
	}

	if (this->clients[fds[i].fd].get_length() == -1)
		this->clients[fds[i].fd].get_request_obj().request_pars(this->clients[fds[i].fd], this->myconfig);
	if (this->clients[fds[i].fd].get_length() >= 0 && !this->clients[fds[i].fd].get_post_boolen())
		pars_post_req(fds[i].fd);
	if (this->clients[fds[i].fd].get_reqs_done())
	{
		std::cout << BLUE << "Request is done" << RESET << std::endl;
		this->fds[i].events = POLLOUT;
	}
}

/*


	nzid time out l pool li tkon chi 75s 
	ndir send tsift buffer b buffer machi kolchi fmera

*/

void Server::handle_response(int i)
{
	std::cout << GREEN << "[" << fds[i].fd << "]" << " : Clinet Response" <<  RESET << std::endl;
	std::string response;
	response = this->clients[fds[i].fd].get_request_obj().buildHttpResponse(this->clients[fds[i].fd].get_keep_alive());
	size_t send_size = send(fds[i].fd, response.c_str(), response.size(), 0);
	this->clients[fds[i].fd].update_activity();
	if (!this->clients[fds[i].fd].get_keep_alive())
	{
		std::cout << RED << ">>>>>>>> 'don't keep alive' <<<<<<<<" <<  RESET << std::endl;
		close(fds[i].fd);
		this->clients.erase(fds[i].fd);
		this->fds.erase(fds.begin() + i);
		return ;
	}
	std::cout << YELLOW << ">>>>>>>> 'keep alive' <<<<<<<<" <<  RESET << std::endl;
	this->clients[fds[i].fd].clean_client_data();
	this->clients[fds[i].fd].get_request_obj().ft_clean();
	this->fds[i].events = POLLIN;
}
